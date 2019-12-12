#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "FixedHeightConstraint.h"
#include "../../Common/Maths.h"

using namespace NCL;
using namespace CSC8503;

FixedHeightConstraint::FixedHeightConstraint(GameObject* _object, const float heightValue)
{
	object = _object;
	height = heightValue;
}

FixedHeightConstraint::~FixedHeightConstraint()
{

}

void FixedHeightConstraint::UpdateConstraint(float dt)
{
	Vector3 position = object->GetTransform().GetWorldPosition();
	position.y = height;
	object->GetTransform().SetWorldPosition(position);
}
