#pragma once
#include "Creature.h"



class Monster : public Creature
{
	using Super = Creature;

public:
	Monster();
	virtual ~Monster();



	virtual void Update() override;


	FVector2D startPosition;
private:
	void UpdateIdle();
	void UpdateMove();
	void UpdateSkill();
	void UpdateDead();

	ULONGLONG _updateTick = 0.2;
	ULONGLONG _nextThinkTick = 0.f;
	ULONGLONG _nextSearchTick = 0.f;
	ULONGLONG _nextUpdateMoveTick = 0.f;

	int _searchCellDist = 15;

	Protocol::MoveState MoveState = Protocol::MoveState::MOVE_STATE_IDLE;
	weak_ptr<GameObject> TargetObject;

	
	FVector2D targetPosition;

	bool bTravel = false;
};

