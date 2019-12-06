#include "GooseObject.h"

int GooseObject::GetItemCount() const
{
	return pickedUpItems.size();
}

int GooseObject::PickUpItem(GameObject* object)
{
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
