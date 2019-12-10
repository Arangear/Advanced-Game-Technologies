#include "PickableObject.h"

void PickableObject::OnCollisionBegin(GameObject* otherObject)
{
	//only goose can move and has Collect resolution
	if (owner == nullptr //Other geese can't steal our apples!
		&& otherObject->GetPhysicsObject()->IsAffectedByGravity() //Other apples can't steal our apples!
		&& otherObject->GetPhysicsObject()->GetCollisionResolution() & CollisionResolution::Collect)
	{
		owner = otherObject;
		id = ((GooseObject*)otherObject)->PickUpItem((GameObject*)this);
		UpdateObjectPosition(id, owner->GetConstTransform().GetWorldPosition());
	}
}
