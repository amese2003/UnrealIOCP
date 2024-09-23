#pragma once
#include <math.h>
//
//struct ANode
//{
//	int F;
//	int G;
//	int Y;
//	int X;
//
//
//	bool operator>=(ANode& other) const
//	{
//		return F >= other.F;
//	}
//
//	bool operator<=(ANode& other) const
//	{
//		return F <= other.F;
//	}
//
//	bool operator==(ANode& other) const
//	{
//		return F == other.F;
//	}
//};

struct FNode
{
	FNode() : _y(0), _x(0) {}
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

struct FVector2
{
	FVector2(int x, int y) : _x(x), _y(y) {}



	int _x, _y;
};


struct FVector
{
	FVector() : _x(0), _y(0), _z(0) {}
	FVector(int x, int y, int z) : _x(x), _y(y), _z(z) {}

	int _x, _y, _z;
};


class GameMap
{
public:
	GameMap();

	FVector WorldPos2Node(float x, float z);
	FVector Node2NodePos(int gridX, int gridZ, bool center = true);
	FVector Node2NodePos(FVector node, bool center) { return Node2NodePos(node._x, node._z, center); }

	float GetGridWidth() { return _rowCount * _tileSize; }
	float GetGridHeight() { return _colCount * _tileSize; }

	FNode& GetNode(int x, int z) { return _nodes[x][z]; }


private:
	int _rowCount = 100, _colCount = 100;
	int _tileSize = 10.f;

	pair<int, int> _pivotPos = { -3000, -3000 };

	vector<vector<FNode>> _nodes;

};