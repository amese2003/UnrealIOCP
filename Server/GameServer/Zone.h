#pragma once

class Player;
class Monster;
class GameObject;

class Zone
{
public:
	Zone(int y, int x) : _y(y), _x(x) {}

	void RemoveObject(GameObjectRef gameObject);

private:
	unordered_set<PlayerRef> _player;
	unordered_set<MonsterRef> _monster;
	int _y, _x;

};

