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
