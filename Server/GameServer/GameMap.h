#pragma once
#include <math.h>

class GameObject;

struct Pos
{
	Pos() : _y(0), _x(0) {}
	Pos(int y, int x) : _y(y), _x(x) {}

	bool operator==(const Pos& other) const
	{
		return _y == other._y && _x == other._x;
	}

	bool operator!=(const Pos& other) const
	{
		return _y != other._y && _x != other._x;
	}

	bool operator<(const Pos& other) const
	{
		return _y * _y + _x * _x < other._y* other._y + other._x * other._x;
	}


	int _y;
	int _x;
};


struct PQNode
{
	int F;
	int G;
	int Y;
	int X;

	PQNode() : F(0), G(0), Y(0), X(0) {}
	PQNode(int f, int g, int y, int x) : F(f), G(g), Y(y), X(x) {}

	bool operator>(const PQNode& other) const
	{
		return F > other.F;
	}

	bool operator<(const PQNode& other) const
	{
		return F < other.F;
	}

	bool operator>=(const PQNode& other) const
	{
		return F >= other.F;
	}

	bool operator<=(const PQNode& other) const
	{
		return F <= other.F;
	}

	bool operator==(const PQNode& other) const
	{
		return F == other.F;
	}
};



struct FVector2D
{
	FVector2D() : _x(0), _y(0) {}
	FVector2D(int x, int y) : _x(x), _y(y) {}

	static FVector2D up() { return FVector2D(0, 1); }
	static FVector2D down() { return FVector2D(0, -1); }
	static FVector2D left() { return FVector2D(-1, 0); }
	static FVector2D right() { return FVector2D(1, 0); }

	int CellDistFromZero() { return abs(_x) + abs(_y); }

	FVector2D operator+ (FVector2D& Other)
	{
		return FVector2D(_x + Other._x, _y + Other._y);
	}

	FVector2D operator- (FVector2D& Other)
	{
		return FVector2D(_x - Other._x, _y - Other._y);
	}

	int _x, _y;
};

struct FNode
{
	FNode() : _y(0), _x(0), CanPass(true) {}
	FNode(int y, int x) : _y(y), _x(x), CanPass(true) {}

	pair<int, int> GetUp() { return { _y + 1, _x }; }
	pair<int, int> GetDown() { return { _y - 1, _x }; }
	pair<int, int> GetLeft() { return { _y, _x - 1 }; }
	pair<int, int> GetRight() { return { _y, _x + 1 }; }

	float Magnitude() { return sqrt(SqrMagnitude()); }
	int SqrMagnitude() { return (_y * _y + _x * _x); }

	int CellDistFromZero() { return abs(_x) + abs(_y); }


	int _y, _x;
	bool CanPass;
};


struct FVector
{
	FVector() : _x(0), _y(0), _z(0) {}
	FVector(int x, int y, int z) : _x(x), _y(y), _z(z) {}

	FVector operator+ (FVector& Other)
	{
		return FVector(_x + Other._x, _y + Other._y, _z + Other._z);
	}

	FVector operator- (FVector& Other)
	{
		return FVector(_x - Other._x, _y - Other._y, _z - Other._z);
	}

	int _x, _y, _z;
};


class GameMap
{
public:
	GameMap();
	
	FVector2D WorldPos2Node(FVector Location);
	FVector2D WorldPos2Node(float x, float z);
	FVector Node2NodePos(FVector2D pos, bool center = true) { return Node2NodePos(pos._x, pos._y, center); }
	FVector Node2NodePos(int gridX, int gridY, bool center = true);
	FVector Node2NodePos(FVector node, bool center) { return Node2NodePos(node._x, node._z, center); }

	float GetGridWidth() { return _rowCount * _tileSize; }
	float GetGridHeight() { return _colCount * _tileSize; }
	int RowCount() { return _rowCount; }
	int ColCount() { return _colCount; }

	FNode& GetNode(int x, int z) { return _nodes[x][z]; }
	bool CanGo(Pos cellPos);

	vector<FVector2D> FindPath(FVector2D start, FVector2D end);
	vector<FVector2D> CalcPathFromParent(map<Pos, Pos> parent, Pos dest);
	int GetDistance(Pos a, Pos b);

	Pos FVector2dToPos(FVector2D cell);
	FVector2D PosToFVector(Pos pos);

	void SetGameObject(int x, int y, shared_ptr<GameObject> TargetObject);
	shared_ptr<GameObject> GetGameObject(int x, int y) { return _gameObjects[x][y]; }

	void ApplyMove(shared_ptr<GameObject> TargetObject, FVector2D Location);

private:
	int _rowCount = 10000, _colCount = 10000;
	int _tileSize = 10.f;

	pair<int, int> _pivotPos = { 0, 0 };

	vector<vector<FNode>> _nodes;
	vector<vector<shared_ptr<GameObject>>> _gameObjects;

	vector<int> _deltaY = { -1, 0, 1, 0, -1, -1, 1, 1 };
	vector<int> _deltaX = { 0, -1, 0, 1, -1, 1, -1, 1 };
	vector<int> _cost = { 10, 10, 10, 10 };
	int _maxDist = 5;
};