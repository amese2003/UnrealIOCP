#pragma once


class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();


public:
	Protocol::ObjectInfo* _objectInfo;
	Protocol::PosInfo* _posInfo;

public:
	atomic<weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
};

