#pragma once
#include "GameMap.h"

class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Update();
	virtual void SetRoom(weak_ptr<Room> targetRoom);
	
	virtual void BroadcastMove();

	weak_ptr<Room> GetRoom();

	/**
	*  CellPos
	*/
	void SetCellPos(FVector Location);
	void SetCellPos(int x, int y);
	void SetCellPos(FVector2D Pos);
	void LocationToCell(FVector Location);
	FVector2D GetCellPos();

	/**
	*  Actual Pos
	*/
	void SetActorLocation(FVector Location);
	FVector GetActorLocation();

public:
	Protocol::ObjectInfo* _objectInfo;
	Protocol::PosInfo* _posInfo;

public:
	atomic<weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
	FVector2D CellPos = { 0, 0 };
	float Speed = 0.f;
};

