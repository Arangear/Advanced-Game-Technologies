#include "GooselandObject.h"

void GooselandObject::OnCollisionBegin(GameObject* otherObject)
{
	if (owner == otherObject //Other geese can't steal our apples!
		&& otherObject->GetPhysicsObject()->IsAffectedByGravity() //Other apples can't steal our apples!
		&& otherObject->GetPhysicsObject()->GetCollisionResolution() & CollisionResolution::Collect)
	{
		for (GameObject*& object : ((GooseObject*)otherObject)->GetItems())
		{
			//handle the object
			((PickableObject*)object)->SetOwner(this);
			items.push_back(object);
			((PickableObject*)object)->SetID(items.size() - 1);
			object->UpdateIslandPosition(items.size() - 1, GetConstTransform().GetWorldPosition());
			((PickableObject*)object)->SetAssigned(true);
			((GooseObject*)otherObject)->AddPoints(((PickableObject*)object)->GetPointsValue());
		}
		((GooseObject*)otherObject)->GetItems().clear();
		((GooseObject*)otherObject)->DropAllCharges();
	}
}
