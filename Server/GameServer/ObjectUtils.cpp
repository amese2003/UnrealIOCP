#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;
atomic<int64> ObjectUtils::monster_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	// [UNUSED(1)][TYPE(7)][ID(24)]
	// [ ........ | ........ | ........ | ........ ]
	int humanId = Protocol::CreatueType::CREATURE_TYPE_PLAYER << 24;
	const int64 newid = humanId | s_idGenerator.fetch_add(1);



	PlayerRef player = make_shared<Player>();
	player->_objectInfo->set_object_id(newid);
	player->_objectInfo->set_object_type(Protocol::OBJECT_TYPE_CREATURE);
	player->_objectInfo->set_speed(20.f);
	player->_posInfo->set_state(Protocol::MOVE_STATE_IDLE);

	player->_posInfo->set_x(Utils::GetRandom(50.f, 100.f));
	player->_posInfo->set_y(Utils::GetRandom(50.f, 100.f));
	player->_posInfo->set_z(100.f);

	player->_session = session;
	session->_player.store(player);

	return player;
}

MonsterRef ObjectUtils::CreateMonster(Protocol::CreatureID  CreatureID)
{
	// [UNUSED(33)][TYPE(7)][ID(24)]
	// [ ........ | ........ | ........ | ........ ]
	int monsterid = (Protocol::CreatueType::CREATURE_TYPE_MONSTER) << 24;
	const int64 newid = monsterid | monster_idGenerator.fetch_add(1);

	MonsterRef monster = make_shared<Monster>();
	monster->_objectInfo->set_object_id(newid);
	monster->_objectInfo->set_object_type(Protocol::OBJECT_TYPE_CREATURE);
	monster->_objectInfo->set_creature_id(CreatureID);
	monster->_objectInfo->set_speed(20.f);
	monster->_posInfo->set_state(Protocol::MOVE_STATE_IDLE);

	return monster;
}

MonsterRef ObjectUtils::CreateDummy()
{
	// [UNUSED(33)][TYPE(7)][ID(24)]
	// [ ........ | ........ | ........ | ........ ]

	MonsterRef monster = make_shared<Monster>();
	monster->_objectInfo->set_object_type(Protocol::OBJECT_TYPE_CREATURE);
	monster->_objectInfo->set_speed(20.f);

	return monster;
}


