#pragma once
#include "PickableObject.h"
#include "NavigationGrid.h"
#include "../CSC8503Common/GameWorld.h"
#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL;
using namespace CSC8503;

class EnemyObject : public GameObject
{
public:
	EnemyObject(string filePath);
	virtual ~EnemyObject() { delete grid; delete stateMachine; }

	void UpdatePosition(GooseObject*& goose, float dt);
	void SetOrigin(Vector3 position)
	{
		origin = position;
	}

	void AddObstacles(vector<GameObject*>& objects) { obstacles = objects; }

	void OnCollisionBegin(GameObject* otherObject);

protected:
	//Chasing
	NavigationGrid* grid;
	vector<Vector3> nodes;
	float speed = 5000.0f;
	Vector3 origin;
	Vector3 targetPosition;
	StateMachine* stateMachine;
	GooseObject* chasedGoose = nullptr;
	float currentDT;
	bool stateSwitch;

	//Raycasting
	vector<GameObject*> obstacles;

	void optimiseNodes();
	bool raycast(Ray& r, RayCollision& closestCollision, bool object = false) const;
	void drawPath();
};