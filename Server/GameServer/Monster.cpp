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

}

void Monster::UpdateIdle()
{
}

void Monster::UpdateMove()
{
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
