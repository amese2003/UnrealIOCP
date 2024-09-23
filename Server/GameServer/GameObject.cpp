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
