#include "pch.h"

#include "GameMap.h"

GameMap::GameMap()
{
	_nodes = vector<vector<FNode>>(_colCount, vector<FNode>(_rowCount));
	_gameObjects = vector<vector<shared_ptr<GameObject>>>(_colCount, vector<shared_ptr<GameObject>>(_rowCount, nullptr));

	for (int col = 0; col < _colCount; col++)
	{
		for (int row = 0; row < _rowCount; row++)
		{
			FNode& node = _nodes[col][row];
			node._y = col;
			node._x = row;
		}
	}
}

FVector2D GameMap::WorldPos2Node(FVector Location)
{
	return WorldPos2Node(Location._x, Location._y);
}

FVector2D GameMap::WorldPos2Node(float x, float y)
{
	FVector2D result;

	FVector gridLocation = FVector(_pivotPos.first, 0, _pivotPos.second);

	float xDelta = x - gridLocation._x;
	float horizon = xDelta / GetGridWidth();
	
	float rowVal = floor(_rowCount * horizon);

	float zDelta = y - gridLocation._y;
	float vertical = zDelta / GetGridHeight();

	int colVal = floor(_colCount * vertical);

	result._x = rowVal;
	result._y = colVal;


	return result;
}

FVector GameMap::Node2NodePos(int gridX, int gridZ, bool center)
{
	FVector result;
	FVector gridLocation = FVector(_pivotPos.first, 0, _pivotPos.second);

	float rowPos = gridX * _tileSize;
	float colPos = gridZ * _tileSize;
	{
		float sum = gridLocation._x;
		float delta = rowPos + sum;

		if (center)
		{
			delta = delta + (_tileSize / 2);
		}

		result._x = delta;
	}

	{
		float sum = gridLocation._z;
		float delta = colPos + sum;

		if (center)
		{
			delta = delta + (_tileSize / 2);
		}
		
		result._z = delta;
	}

	return result;
}

bool GameMap::CanGo(Pos cellPos)
{
	if (cellPos._y < 0 || cellPos._y > _colCount)
		return false;

	if (cellPos._x < 0 || cellPos._x > _rowCount)
		return false;

	return true;
}

vector<FVector2D> GameMap::FindPath(FVector2D start, FVector2D end)
{
	// F = G + H
	// F = 최종 점수
	// G = 시작점에서 해당좌표 비용
	// H = 목적지까지 얼마나 가깝나

	set<Pos> closeList;


	map<Pos, int> openList;
	map<Pos, Pos> parent;

	priority_queue<PQNode> pq;

	Pos pos = FVector2dToPos(start);
	Pos dest = FVector2dToPos(end);

	openList[pos] = 10 * (abs(dest._y - pos._y) + abs(dest._x - pos._x));
	pq.push(PQNode(abs(dest._y - pos._y) + abs(dest._x - pos._x), 0, pos._y, pos._x));
	parent[pos] = pos;

	while (pq.size() > 0)
	{
		PQNode pqNode = pq.top(); pq.pop();
		Pos node = Pos(pqNode.Y, pqNode.X);

		if (closeList.find(node) != closeList.end())
			continue;

		closeList.insert(node);

		if (node._y == dest._y && node._x == dest._x)
			break;

		for (int i = 0; i < _deltaY.size(); i++)
		{
			Pos next = Pos(node._y + _deltaY[i], node._x + _deltaX[i]);

			if (abs(pos._y - pos._x) + abs(pos._x - next._y) > _maxDist)
				continue;

			if (next._y != dest._y || next._x != dest._x)
			{
				if (CanGo(next) == false)
					continue;
			}

			if (closeList.contains(next))
				continue;

			int g = pqNode.G + GetDistance(node, next);
			int h = GetDistance(next, dest);
			int value = openList.count(next) ? openList[next] : 210000000;

			if (value < g + h)
				continue;


			if (openList.find(next) == openList.end())
			{
				openList.insert({ next, g + h });
			}

			if (parent.find(next) != parent.end())
			{
				parent[next] = node;
			}

			pq.push(PQNode(g + h, g, next._x, next._y));
		}
	}

	return CalcPathFromParent(parent, dest);
}

vector<FVector2D> GameMap::CalcPathFromParent(map<Pos, Pos> parent, Pos dest)
{
	vector<FVector2D> cells;

	if (parent.find(dest) == parent.end())
	{
		Pos best = Pos(0, 0);
		int bestDest = 210000000;

		for (auto pat : parent)
		{
			int dist = abs(dest._x - pat.second._x) + abs(dest._y - pat.second._y);

			if (dist < bestDest)
			{
				best = pat.second;
				bestDest = dist;
			}
		}

		dest = best;
	}

	Pos pos = dest;
	while (parent[pos] != pos)
	{
		cells.push_back(PosToFVector(pos));
		pos = parent[pos];
	}

	cells.push_back(PosToFVector(pos));

	return cells;
}

int GameMap::GetDistance(Pos a, Pos b)
{
	int dstX = abs(a._x - b._x);
	int dstY = abs(a._y - b._y);

	if (dstX > dstY)
		return 14 * dstY + 10 * (dstX - dstY);

	return 14 * dstX + 10 * (dstY - dstX);
}

Pos GameMap::FVector2dToPos(FVector2D cell)
{
	return Pos(cell._x, cell._y);
}

FVector2D GameMap::PosToFVector(Pos pos)
{
	return FVector2D(pos._x, pos._y);
}

void GameMap::SetGameObject(int x, int y, shared_ptr<GameObject> TargetObject)
{
	_gameObjects[x][y] = TargetObject;
}

void GameMap::ApplyMove(shared_ptr<GameObject> TargetObject, FVector Location)
{
}
