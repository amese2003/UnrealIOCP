#pragma once
#include "JobQueue.h"
#include "GameMap.h"
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
	// ������ ���� �������� ��
	bool HandleEnterPlayer(PlayerRef player);
	bool HandleLeavePlayer(PlayerRef player);
	void HandleMove(Protocol::C_MOVE pkt);

	int GetObjectTypes(GameObjectRef object);

	void BeginPlay();
	void UpdateTick();

	RoomRef GetRoomRef();


	void SpawnMonster(Protocol::CreatureID CreatureID);

	weak_ptr<GameObject> FindClosetPlayer(FVector2D CellPos, int searchCellDist);
	void Broadcast(SendBufferRef sendBuffer, uint64 exceptId = 0);
private:
	bool AddObject(GameObjectRef player);
	bool RemoveObject(uint64 objectId);

	//void SpawnMonster();
	USE_LOCK;


private:
	


public:
	shared_ptr<GameMap> _gameMap;

	map<uint64, GameObjectRef> _players;
	map<uint64, MonsterRef> _monsters;


	MonsterRef _dummy;

	int _zoneCells = 10;
};

extern RoomRef GRoom;