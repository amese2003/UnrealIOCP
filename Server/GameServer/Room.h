#pragma once
#include "JobQueue.h"
#include "Enum.pb.h"

class R3CharacterBase;
class GameSession;
class GameMap;

class Room : public JobQueue
{
public:
	Room();
	virtual ~Room();

	


public:
	// 유저가 새로 등장했을 때
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C_MOVE pkt);

	int GetObjectTypes(GameObjectRef object);

	void BeginPlay();
	void UpdateTick();

	RoomRef GetRoomRef();


	void SpawnMonster(Protocol::CreatureID CreatureID);

private:
	bool AddObject(GameObjectRef player);
	bool RemoveObject(uint64 objectId);

	//void SpawnMonster();
	USE_LOCK;


private:
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);


public:
	shared_ptr<GameMap> _gameMap;

	map<uint64, GameObjectRef> _players;
	map<uint64, MonsterRef> _monsters;

	int _zoneCells = 10;
};

extern RoomRef GRoom;