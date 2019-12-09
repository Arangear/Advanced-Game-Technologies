#pragma once
#include "GooseObject.h"
#include "NavigationGrid.h"

using namespace NCL;
using namespace CSC8503;

class EnemyObject : public GameObject
{
public:
	EnemyObject(string filePath) : GameObject() { grid = new NavigationGrid(filePath); }
	virtual ~EnemyObject() { delete grid; }

	void UpdatePosition(GooseObject*& goose, float dt);
protected:
	NavigationGrid* grid;
	vector<Vector3> nodes;
	float speed = 30.0f;
};