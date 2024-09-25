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

};

