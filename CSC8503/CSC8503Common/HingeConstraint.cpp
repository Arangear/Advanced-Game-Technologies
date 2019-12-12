#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "HingeConstraint.h"
#include "../../Common/Maths.h"

using namespace NCL;
using namespace CSC8503;

HingeConstraint::HingeConstraint(GameObject* a, GameObject* b)
{
	objectA = a;
	objectB = b;
}

HingeConstraint::~HingeConstraint()
{

}

void HingeConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePos = objectA->GetConstTransform().GetWorldPosition() - objectB->GetConstTransform().GetWorldPosition();
	relativePos.y = 0;
	Vector3 dir = relativePos.Normalised();

	//Manage rotation
	objectA->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(0,-Maths::RadiansToDegrees(atan2f(dir.z, dir.x)), 0));
	objectB->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(0, Maths::RadiansToDegrees(atan2f(-dir.z, dir.x)), 0));
}
