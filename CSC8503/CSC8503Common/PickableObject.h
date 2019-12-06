#pragma once
#include "GameObject.h"
#include "GooseObject.h"

using namespace NCL;
using namespace CSC8503;

class PickableObject : public GameObject
{
public:
	PickableObject() : GameObject() {}
	virtual ~PickableObject() {}

	void OnCollisionBegin(GameObject* otherObject);
protected:
	GameObject* owner = nullptr;
	int id;
};