#pragma once
#include "PickableObject.h"

using namespace NCL;
using namespace CSC8503;

class GooselandObject : public GameObject
{
public:
	GooselandObject() : GameObject() {}
	virtual ~GooselandObject() {}

	void OnCollisionBegin(GameObject* otherObject);
	
	GooseObject* GetOwner() const
	{
		return owner;
	}

	void SetOwner(GooseObject* goose)
	{
		owner = goose;
	}

protected:
	GooseObject* owner = nullptr;
	vector<GameObject*> items;
};