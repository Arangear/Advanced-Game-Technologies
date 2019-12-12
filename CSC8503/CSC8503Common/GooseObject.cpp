#include "GooseObject.h"

int GooseObject::GetItemCount() const
{
	return pickedUpItems.size();
}

int GooseObject::PickUpItem(GameObject* object)
{
	//If special object was stolen
	if (object->GetRenderObject()->GetColour() == Vector4(1.0f, 1.0f, 0.0f, 1.0f))
	{
		commitedTheft = true;
	}
	pickedUpItems.push_back(object);
	return pickedUpItems.size() - 1;
}

void GooseObject::UpdatePositions()
{
	for(int i=0;i<pickedUpItems.size();i++)
	{
		pickedUpItems[i]->UpdateObjectPosition(i + 1, GetConstTransform().GetWorldPosition());
	}
}

void GooseObject::ManageAbilities(const float dt)
{
	if (sprint <= 0.0)
	{
		speedMultiplier = 2;
	}
	if (sprint > 0)
	{
		sprint -= dt;
	}
	if (sprintCD > 0)
	{
		sprintCD -= dt;
	}
	if (quackAttackCD > 0)
	{
		quackAttackCD -= dt;
	}
}