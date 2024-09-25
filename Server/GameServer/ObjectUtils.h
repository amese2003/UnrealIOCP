#pragma once
#include "Enum.pb.h"

class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);
	static MonsterRef CreateMonster(Protocol::CreatureID  CreatureID);


private:
	static atomic<int64> s_idGenerator;
	static atomic<int64> monster_idGenerator;
};

