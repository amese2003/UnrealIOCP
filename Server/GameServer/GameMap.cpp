#include "pch.h"

#include "GameMap.h"

GameMap::GameMap()
{
	_nodes = vector<vector<FNode>>(_colCount, vector<FNode>(_rowCount));

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

FVector GameMap::WorldPos2Node(float x, float z)
{
	FVector result;

	FVector gridLocation = FVector(_pivotPos.first, 0, _pivotPos.second);

	float xDelta = x - gridLocation._x;
	float horizon = xDelta / GetGridWidth();
	
	float rowVal = floor(_rowCount * horizon);

	float zDelta = z - gridLocation._z;
	float vertical = zDelta / GetGridHeight();

	int colVal = floor(_colCount * vertical);

	result._x = rowVal;
	result._z = colVal;


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
