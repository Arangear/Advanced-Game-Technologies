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

protected:
	vector<GameObject*> pickedUpItems;
};