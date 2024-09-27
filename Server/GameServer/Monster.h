#pragma once
#include "Creature.h"



class Monster : public Creature
{
	using Super = Creature;

public:
	Monster();
	virtual ~Monster();



	virtual void Update() override;



private:
	void UpdateIdle();
	void UpdateMove();
	void UpdateSkill();
	void UpdateDead();

	long _updateTick = 0.2;
	long _nextMoveTick = 0.f;
	long _nextSearchTick = 0.f;

	int _searchCellDist = 5;

	Protocol::MoveState MoveState = Protocol::MoveState::MOVE_STATE_IDLE;
	weak_ptr<GameObject> TargetObject;

	FVector2D targetPosition;
};

