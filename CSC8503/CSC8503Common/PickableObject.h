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

	void SetOwner(GameObject* _owner)
	{
		owner = _owner;
	}

	GameObject* GetOwner() const
	{
		return owner;
	}

	int GetID() const
	{
		return id;
	}

	void SetID(int value)
	{
		id = value;
	}

protected:
	GameObject* owner = nullptr;
	int id;
};