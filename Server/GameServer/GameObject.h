#pragma once


class GameObject : public enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void SetRoom(weak_ptr<Room> targetRoom);
	weak_ptr<Room> GetRoom();

public:
	Protocol::ObjectInfo* _objectInfo;
	Protocol::PosInfo* _posInfo;

public:
	atomic<weak_ptr<Room>> _room;

protected:
	bool _isPlayer = false;
};

