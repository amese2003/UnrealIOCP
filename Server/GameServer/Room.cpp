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

RoomRef GRoom = make_shared<Room>();

Room::Room()
{
	//_gameMap = make_shared<GameMap>();
}

Room::~Room()
{
}

void Room::UpdateTick()
{
	//cout << "Update Room" << endl;
	DoTimer(100, &Room::UpdateTick);
}



bool Room::HandleEnterPlayer(PlayerRef player)
{
	bool success = AddObject(player);

	// 랜덤 위치
	player->_posInfo->set_object_id(player->_objectInfo->object_id());
	player->_posInfo->set_x(player->_posInfo->x());
	player->_posInfo->set_y(player->_posInfo->y());
	player->_posInfo->set_z(player->_posInfo->z());
	player->_posInfo->set_yaw(player->_posInfo->yaw());

	// 입장 사실을 신입 플레이어에게 알린다
	{
		Protocol::S_ENTER_GAME enterGamePkt;
		enterGamePkt.set_success(success);

		Protocol::ObjectInfo* playerInfo = new Protocol::ObjectInfo();
		playerInfo->CopyFrom(*player->_objectInfo);
		enterGamePkt.set_allocated_player(playerInfo);

		int objectid = playerInfo->object_id();
		//enterGamePkt.release_player();

		
		//for (auto& item : _monsters)
		//{
		//	Protocol::PlayerInfo* playerInfo = enterGamePkt.add_monsters();
		//	playerInfo->CopyFrom(*item.second->_playerInfo);
		//}

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

	// 에러에러에러에러에러에러에러엘
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



RoomRef Room::GetRoomRef()
{
	return static_pointer_cast<Room>(shared_from_this());
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
	player->_room.store(GetRoomRef());

	return true;
}

bool Room::RemoveObject(uint64 objectId)
{
	// 없다면 문제가 있다.
	if (_players.find(objectId) == _players.end())
		return false;

	GameObjectRef player = _players[objectId];
	player->_room.store(weak_ptr<Room>());

	_players.erase(objectId);

	return true;
}

//void Room::SpawnMonster()
//{
//	MonsterRef monster = ObjectUtils::CreateMonster();
//
//	// 랜덤 위치
//	monster->_playerInfo->set_x(Utils::GetRandom(0.f, 500.f));
//	monster->_playerInfo->set_y(Utils::GetRandom(0.f, 500.f));
//	monster->_playerInfo->set_z(50);
//	monster->_playerInfo->set_yaw(Utils::GetRandom(0.f, 100.f));
//
//	const uint64 objectId =  monster->_playerInfo->object_id();
//
//	//monster->_room.store(GetRoomRef());
//	_monsters[objectId] = monster;
//
//	//
//}
