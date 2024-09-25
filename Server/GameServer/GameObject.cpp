#include "pch.h"
#include "GameObject.h"

GameObject::GameObject()
{
	_objectInfo = new Protocol::ObjectInfo();
	_posInfo = new Protocol::PosInfo();
	_objectInfo->set_allocated_pos_info(_posInfo);
}

GameObject::~GameObject()
{
	delete _objectInfo;
}

void GameObject::Update()
{
}

void GameObject::SetRoom(weak_ptr<Room> targetRoom)
{
	_room.store(targetRoom);
}

weak_ptr<Room> GameObject::GetRoom()
{
	if (_room.load().lock() != nullptr)
		return _room;

	return weak_ptr<Room>();
}
