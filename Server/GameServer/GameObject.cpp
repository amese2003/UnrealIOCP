#include "pch.h"
#include "GameObject.h"
#include "Room.h"

GameObject::GameObject()
{
	_objectInfo = new Protocol::ObjectInfo();
	_posInfo = new Protocol::PosInfo();
	_objectInfo->set_allocated_pos_info(_posInfo);
}

GameObject::~GameObject()
{
	delete _objectInfo;
	delete _posInfo;
}

void GameObject::Update()
{
}

void GameObject::SetRoom(weak_ptr<Room> targetRoom)
{
	_room.store(targetRoom);
}

void GameObject::BroadcastMove()
{
	Protocol::S_MOVE movePacket;
}

weak_ptr<Room> GameObject::GetRoom()
{
	if (_room.load().lock() != nullptr)
		return _room;

	return weak_ptr<Room>();
}

void GameObject::SetSpeed(float speed)
{
	Speed = speed;
}

void GameObject::SetCellPos(FVector Location)
{
	CellPos = _room.load().lock()->_gameMap->WorldPos2Node(Location);
}

void GameObject::SetCellPos(int x, int y)
{
	CellPos = FVector2D(x, y);
}

void GameObject::SetCellPos(FVector2D Pos)
{
	CellPos = Pos;
}

void GameObject::LocationToCell(FVector Location)
{
	
}

FVector2D GameObject::GetCellPos()
{
	return CellPos;
}

void GameObject::SetActorLocation(FVector Location, bool WithCell)
{
	_posInfo->set_x(Location._x);
	_posInfo->set_y(Location._y);
	_posInfo->set_z(Location._z);

	if (WithCell == true && _room.load().lock() != nullptr)
	{
		FVector2D currentNode = _room.load().lock()->_gameMap->WorldPos2Node(Location);
		CellPos = currentNode;
	}
}

FVector GameObject::GetActorLocation()
{
	return FVector(_posInfo->x(), _posInfo->y(), _posInfo->z());
}
