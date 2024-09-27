#include "pch.h"
#include "Monster.h"
#include "Room.h"
#include "Utils.h"

Monster::Monster()
{

}

Monster::~Monster()
{
	
}

void Monster::Update()
{
	Super::Update();

	ULONGLONG curtick = ::GetTickCount64();

	if (_nextThinkTick > curtick)
		return;

	_nextThinkTick = curtick + 200;

	switch (MoveState)
	{
	case Protocol::MoveState::MOVE_STATE_IDLE:
		UpdateIdle();
		break;

	case Protocol::MoveState::MOVE_STATE_RUN:
		UpdateMove();
		break;
	}

}

void Monster::UpdateIdle()
{
	auto tick = ::GetTickCount64();

	if (_nextSearchTick > tick)
		return;

	_nextSearchTick = tick + 1000;

	weak_ptr<GameObject> target = _room.load().lock()->_dummy;

	if (target.lock() == nullptr)
		return;


	TargetObject = target;
	MoveState = Protocol::MoveState::MOVE_STATE_RUN;
}

void Monster::UpdateMove()
{
	auto tick = ::GetTickCount64();
	if (_nextUpdateMoveTick > tick)
		return;

	int moveTick = (int)(1000 / Speed);
	_nextUpdateMoveTick = tick + moveTick;

	FVector2D DestTarget = _room.load().lock()->_dummy->GetCellPos();

	vector<FVector2D> path;
	
	if (bTravel == false)
		path = _room.load().lock()->_gameMap->FindPath(CellPos, DestTarget);
	else
		path = _room.load().lock()->_gameMap->FindPath(CellPos, startPosition);

	cout << "test breakpoint" << endl;

	if (path.size() > 1)
		_room.load().lock()->_gameMap->ApplyMove(shared_from_this(), path[1]);
	
	int a = (startPosition - CellPos).SqrMagnitude();

	if (a > 16 && bTravel == false)
		bTravel = !bTravel;

	if (a < 1 && bTravel == true)
		bTravel = !bTravel;
	
	{
		Protocol::S_MOVE movePkt;
		{
			Protocol::PosInfo* info = movePkt.mutable_info();
			info->CopyFrom(*_posInfo);

			movePkt.set_creature_type(Protocol::CREATURE_TYPE_MONSTER);
		}

		SendBufferRef sendBuffer = ServerPacketHandler::MakeSendBuffer(movePkt);
		_room.load().lock()->Broadcast(sendBuffer);
	}
}

void Monster::UpdateSkill()
{
}

void Monster::UpdateDead()
{
}

//void Monster::Move()
//{
//
//	float deltaX = Utils::GetRandom(-100.f, 150.f);
//	float deltaY = Utils::GetRandom(-100.f, 150.f);
//
//	//_posInfo->set_x(_playerInfo->x() + deltaX);
//	//_posInfo->set_y(_playerInfo->y() + deltaY);
//	//_posInfo->set_state(Protocol::MOVE_STATE_RUN);
//	////_room.load().lock()->HandleMonsterMove(_playerInfo);
//}
