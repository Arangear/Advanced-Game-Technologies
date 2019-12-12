#pragma once
#include "Constraint.h"

using namespace NCL::Maths;

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class HingeConstraint : public Constraint
		{
		public:
			HingeConstraint(GameObject* a, GameObject* b, const Vector3& axis, const Vector3& direction);
			~HingeConstraint();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;

			Vector3 constraintAxis;
			Vector3 direction;
		};
	}
}