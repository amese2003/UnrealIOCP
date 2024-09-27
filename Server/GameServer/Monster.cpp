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

	if (_nextMoveTick > ::GetTickCount64())
		return;

	_nextMoveTick += ::GetTickCount64() + 200;

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
	/*auto tick = ::GetTickCount64();

	if (_nextSearchTick > tick)
		return;

	_nextSearchTick = tick + 1000;

	weak_ptr<GameObject> target = _room.load().lock()->_dummy;

	if (target.lock() == nullptr)
		return;


	TargetObject = target;
	MoveState = Protocol::MoveState::MOVE_STATE_RUN;*/
}

void Monster::UpdateMove()
{
	auto tick = ::GetTickCount64();
	if (_nextMoveTick > tick)
		return;

	int moveTick = (int)(1000 / Speed);
	_nextMoveTick = tick + moveTick;

	vector<FVector2D> path = _room.load().lock()->_gameMap->FindPath(CellPos, _room.load().lock()->_dummy->GetCellPos());



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
