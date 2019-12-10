#pragma once
#include "GameObject.h"
#include "GooseObject.h"

using namespace NCL;
using namespace CSC8503;

class PickableObject : public GameObject
{
public:
	PickableObject(unsigned int pointsValue) : GameObject(), pointsValue(pointsValue) {}
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

	const unsigned int GetPointsValue() const
	{
		return pointsValue;
	}

	void SetAssigned(const bool value)
	{
		assigned = value;
	}

	bool GetAssigned() const
	{
		return assigned;
	}

	void ResetPosition()
	{
		this->GetTransform().SetWorldPosition(origin);
	}

	void SetOrigin(const Vector3 position)
	{
		origin = position;
	}

protected:
	const unsigned int pointsValue;
	GameObject* owner = nullptr;
	int id = -1;
	bool assigned = false;
	Vector3 origin;
};