#pragma once

class Player;
class GameObject;

class VisionCube
{
	VisionCube(PlayerRef owner) : _owner(owner) {}

	PlayerRef GetOwner() { return _owner.lock(); }
	unordered_set<GameObjectRef> GatherObjects();

private:
	weak_ptr<Player> _owner;
	unordered_set<GameObjectRef> _previousObject;
};

