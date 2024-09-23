#include "pch.h"
#include "ObjectUtils.h"
#include "Player.h"
#include "Monster.h"
#include "GameSession.h"

atomic<int64> ObjectUtils::s_idGenerator = 1;

PlayerRef ObjectUtils::CreatePlayer(GameSessionRef session)
{
	int humanId = 1 << 23;
	const int64 newid = humanId | s_idGenerator.fetch_add(1);



	PlayerRef player = make_shared<Player>();
	player->_objectInfo->set_object_id(newid);

	player->_session = session;
	session->_player.store(player);

	return player;
}

