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
	int humanId = 1 << 24;
	const int64 newid = humanId | s_idGenerator.fetch_add(1);



	PlayerRef player = make_shared<Player>();
	player->_objectInfo->set_object_id(newid);

	player->_session = session;
	session->_player.store(player);

	return player;
}

MonsterRef ObjectUtils::CreateMonster(int monsterTypes)
{
	// [UNUSED(33)][TYPE(7)][ID(24)]
	// [ ........ | ........ | ........ | ........ ]
	int monsterid = monsterTypes << 24;
	const int64 newid = monsterid | s_idGenerator.fetch_add(1);

	MonsterRef monster = make_shared<Monster>();
	monster->_objectInfo->set_object_id(newid);

	return monster;
}

