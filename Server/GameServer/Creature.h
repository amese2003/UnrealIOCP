#pragma once
#include "GameObject.h"
#include "Enum.pb.h"


class Creature : public GameObject
{
	using Super = GameObject;

public:
	Creature();
	virtual ~Creature();


protected:
	Protocol::MoveState _moveState = Protocol::MoveState::MOVE_STATE_IDLE;

};

