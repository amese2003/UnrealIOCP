#pragma once
#include "Creature.h"



class Monster : public Creature
{
public:
	Monster();
	virtual ~Monster();



	void Update();
	void Move();

public:
	bool _busy = false;
};

