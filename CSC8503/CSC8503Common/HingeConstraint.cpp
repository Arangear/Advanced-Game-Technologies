#include "PositionConstraint.h"
#include "../../Common/Vector3.h"
#include "GameObject.h"
#include "Debug.h"
#include "HingeConstraint.h"

using namespace NCL;
using namespace CSC8503;

HingeConstraint::HingeConstraint(GameObject* a, GameObject* b, const Vector3& axis, const Vector3& _direction)
{
	objectA = a;
	objectB = b;
	constraintAxis = axis;
	direction = _direction;
}

HingeConstraint::~HingeConstraint()
{

}

//a simple constraint that stops objects from being more than <distance> away
//from each other...this would be all we need to simulate a rope, or a ragdoll
void HingeConstraint::UpdateConstraint(float dt)
{
	Vector3 relativePosition = objectA->GetConstTransform().GetWorldPosition() - objectB->GetConstTransform().GetWorldPosition();
	relativePosition.y = 0;

	Vector3 currentDirection = relativePosition.Normalised();

	Vector3 offset = direction - currentDirection;

	if (offset != Vector3(0, 0, 0))
	{
		Vector3 offsetDir = -offset.Normalised();

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 relativeVelocity = physA->GetAngularVelocity() - physB->GetAngularVelocity();

		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass > 0.0f)
		{
			//how much of their relative force is affecting the constraint
			float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);
			float biasFactor = 0.01f;
			Vector3 bias = offset * -(biasFactor / dt);

			Vector3 lambda = -(Vector3(velocityDot, velocityDot, velocityDot) + bias) / constraintMass;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyAngularImpulse(Vector3(0, -aImpulse.x, 0)); // multiplied by mass here
			physB->ApplyAngularImpulse(Vector3(0, -bImpulse.x, 0)); // multiplied by mass here
		}
	}
}
