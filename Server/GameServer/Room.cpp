#include "pch.h"
#include "Room.h"
#include "GameSession.h"
#include "Protocol.pb.h"
#include "ServerPacketHandler.h"
#include "Player.h"
#include "Monster.h"
#include "ObjectUtils.h"
#include "GameSession.h"
#include "GameObject.h"
#include "GameMap.h"

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
	//_gameMap = make_shared<GameMap>();

	
}

Room::~Room()
{
}

void Room::BeginPlay()
{
	_gameMap = make_shared<GameMap>();

	for (int i = 0; i < 1; i++)
		SpawnMonster(Protocol::CreatureID::MONSTER_TYPE_WOLF);
	

	FVector SpawnLocation(Utils::GetRandom(30.f, 50.f), Utils::GetRandom(30.f, 50.f), 100.f);
	_dummy = ObjectUtils::CreateDummy();
	_dummy->_room = static_pointer_cast<Room>(shared_from_this());
	_dummy->SetActorLocation(SpawnLocation);
	_dummy->SetCellPos(SpawnLocation);
	
}

void Room::UpdateTick()
{
	for (pair<uint64, MonsterRef> monster : _monsters)
	{
		monster.second->Update();
	}

	cout << "Update Room" << endl;
	DoTimer(1000, &Room::UpdateTick);
}





RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
}



bool Room::HandleEnterPlayer(PlayerRef player)
{
	bool success = AddObject(player);
	
	FVector PlayerLocation = player->GetActorLocation();

	// 랜덤 위치
	player->_posInfo->set_object_id(player->_objectInfo->object_id());
	player->SetActorLocation(PlayerLocation);
	player->_posInfo->set_yaw(player->_posInfo->yaw());

	// 입장 사실을 신입 플레이어에게 알린다
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(*player->_objectInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		int objectid = playerInfo->object_id();

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(enterGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// 입장 사실을 다른 플레이어에게 알린다
	{
		Protocol::S_SPAWN spawnPkt;

		Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
		playerInfo->CopyFrom(*player->_objectInfo);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer, player->_objectInfo->object_id());
	}

	// 기존 입장한 플레이어 목록을 신입 플레이어한테 전송해준다
	{
		Protocol::S_SPAWN spawnPkt;

		for (auto& item : _players)
		{
			Protocol::ObjectInfo* playerInfo = spawnPkt.add_players();
			playerInfo->CopyFrom(*item.second->_objectInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// 몬스터 스폰 패킷
	{
		Protocol::S_MONSTERSPAWN spawnPkt;

		for (auto& item : _monsters)
		{
			Protocol::ObjectInfo* playerInfo = spawnPkt.add_monsters();
			playerInfo->CopyFrom(*item.second->_objectInfo);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
		Broadcast(sendBuffer);
	}

	return success;
}

bool Room::HandleLeavePlayer(PlayerRef player)
{
	if (player == nullptr)
		return false;

	WRITE_LOCK;

	const uint64 objectId = player->_objectInfo->object_id();
	bool success = RemoveObject(objectId);

	// 퇴장 사실을 퇴장하는 플레이어에게 알린다
	{
		Protocol::S_LEAVE_GAME leaveGamePkt;

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(leaveGamePkt);
		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	// 퇴장 사실을 알린다
	{
		Protocol::S_DESPAWN despawnPkt;
		despawnPkt.add_object_ids(objectId);

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(despawnPkt);
		Broadcast(sendBuffer, objectId);

		if (auto session = player->_session.lock())
			session->Send(sendBuffer);
	}

	return success;
}


void Room::HandleMove(Protocol::C_MOVE pkt)
{
	const uint64 objectId = pkt.info().object_id();
	if (_players.find(objectId) == _players.end())
		return;

	// 적용

	GameObjectRef& player = _players[objectId];
	player->_posInfo->CopyFrom(pkt.info());

	

	// 이동 
	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(pkt.info());
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		Broadcast(sendBuffer);
	}
}

int Room::GetObjectTypes(GameObjectRef object)
{
	if (object)
	{
		if (Protocol::ObjectInfo* targetInfo = object->_objectInfo)
		{
			uint64 objectid =  targetInfo->object_id();
			return objectid >> 24;
		}
	}

	// Invalid
	return 0;
}


void Room::Broadcast(SendBufferRef sendBuffer, uint64 exceptId)
{
	for (auto& item : _players)
	{
		PlayerRef player = static_pointer_cast<Player>(item.second);
		if (player->_objectInfo->object_id() == exceptId)
			continue;

		player->_session.lock();

		if (GameSessionRef session = player->_session.lock())
			session->Send(sendBuffer);


		//cout << "유저 제외 통보 : " << exceptId << " 기존유저 : " << player->_playerInfo->object_id() << endl;
	}
}

bool Room::AddObject(GameObjectRef player)
{
	// 중첩?
	if (_players.find(player->_objectInfo->object_id()) != _players.end())
		return false;


	_players.insert(make_pair(player->_objectInfo->object_id(), player));
	player->SetRoom(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 objectId)
{
	// 없다면 문제가 있다.
	if (_players.find(objectId) == _players.end())
		return false;

	GameObjectRef player = _players[objectId];
	player->SetRoom(weak_ptr<Room>());

	_players.erase(objectId);

	return true;
}

void Room::SpawnMonster(Protocol::CreatureID CreatureID)
{
	MonsterRef monster = ObjectUtils::CreateMonster(CreatureID);

	/*PosInfo->set_x(Utils::GetRandom(0.f, 10.f));
	posInfo->set_y(Utils::GetRandom(0.f, 10.f));
	posInfo->set_z(100.f);*/
	//// 랜덤 위치
	FVector SpawnLocation(Utils::GetRandom(90.f, 100.f), Utils::GetRandom(90.f, 100.f), 100.f);
	monster->_room.store(GetRoomRef());

	monster->SetActorLocation(SpawnLocation);
	_monsters[monster->_objectInfo->object_id()] = monster;
	monster->SetSpeed(monster->_objectInfo->speed());

	Protocol::S_MONSTERSPAWN spawnPkt;

	Protocol::ObjectInfo* playerInfo = spawnPkt.add_monsters();
	playerInfo->CopyFrom(*monster->_objectInfo);

	SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(spawnPkt);
	Broadcast(sendBuffer);
}

weak_ptr<GameObject> Room::FindClosetPlayer(FVector2D CellPos, int searchCellDist)
{
	weak_ptr<GameObject> target;

	int startX = CellPos._x - searchCellDist;
	int startY = CellPos._y - searchCellDist;

	int endX = CellPos._x + searchCellDist;
	int endY = CellPos._y + searchCellDist;


	vector<weak_ptr<GameObject>> FindTargets;
	for (int x = startX; x < endX; x++)
	{
		for (int y = startY; y < endY; y++)
		{
			if (x < 0 || x > _gameMap->RowCount())
				continue;

			if (y < 0 || y > _gameMap->ColCount())
				continue;

			if (auto targetGameObject = _gameMap->GetGameObject(x, y))
			{
				FindTargets.push_back(targetGameObject);
			}
		}
	}

	auto sorta = [&](weak_ptr<GameObject> a, weak_ptr<GameObject> b) {
		FVector2D aCell = a.lock().get()->GetCellPos();
		FVector2D bCell = b.lock().get()->GetCellPos();

		int leftDist = (aCell - CellPos).CellDistFromZero();
		int rightDist = (bCell - CellPos).CellDistFromZero();

		return leftDist - rightDist;
	};

	sort(FindTargets.begin(), FindTargets.end(), sorta);

	for (auto FindTarget : FindTargets)
	{
		vector<FVector2D> path = _gameMap->FindPath(CellPos, FindTarget.lock()->GetCellPos());

		if (path.size() < 2 || path.size() > searchCellDist)
			continue;

		return FindTarget;
	}

	return target;
}
