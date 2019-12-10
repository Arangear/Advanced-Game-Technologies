#pragma once
#include "PickableObject.h"
#include "NavigationGrid.h"
#include "../CSC8503Common/GameWorld.h"

using namespace NCL;
using namespace CSC8503;

class EnemyObject : public GameObject
{
public:
	EnemyObject(string filePath) : GameObject() { grid = new NavigationGrid(filePath); }
	virtual ~EnemyObject() { delete grid; }

	void UpdatePosition(GooseObject*& goose, float dt);
	void SetOrigin(Vector3 position)
	{
		origin = position;
	}

	void AddObstacles(vector<GameObject*>& objects) { obstacles = objects; }

	void OnCollisionBegin(GameObject* otherObject);

protected:
	NavigationGrid* grid;
	vector<Vector3> nodes;
	float speed = 4000.0f;
	Vector3 origin;
	Vector3 targetPosition;

	//Raycasting
	vector<GameObject*> obstacles;

	void optimiseNodes();
	bool raycast(Ray& r, RayCollision& closestCollision, bool object = false) const;
	void drawPath();
};