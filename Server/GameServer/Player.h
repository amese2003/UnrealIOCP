#pragma once
#include "Creature.h"

class GameSession;
class Room;
class VisionCube;

class Player : public Creature
{
	using Super = Creature;

public:
	Player();
	virtual ~Player();

	weak_ptr<GameSession> _session;
	
	shared_ptr<VisionCube> _vision;
};

