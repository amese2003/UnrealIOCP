#pragma once


class ObjectUtils
{
public:
	static PlayerRef CreatePlayer(GameSessionRef session);
	static MonsterRef CreateMonster(int monsterTypes);


private:
	static atomic<int64> s_idGenerator;
	static atomic<int64> monster_idGenerator;
};

