#pragma once
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class GooseObject : public GameObject
{
public:
	GooseObject() : GameObject() {}
	virtual ~GooseObject() {}

	int GetItemCount() const;

	//Returns ID of the picked up object;
	int PickUpItem(GameObject* object);

	void UpdatePositions();

	void AddPoints(const unsigned int value)
	{
		points += value;
	}

	unsigned int GetPoints() const
	{
		return points;
	}

	vector<GameObject*>& GetItems()
	{
		return pickedUpItems;
	}
protected:
	vector<GameObject*> pickedUpItems;
	unsigned int points = 0;
};