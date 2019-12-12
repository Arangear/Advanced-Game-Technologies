#pragma once
#include "Constraint.h"

using namespace NCL::Maths;

namespace NCL
{
	namespace CSC8503
	{
		class GameObject;

		class FixedHeightConstraint : public Constraint
		{
		public:
			FixedHeightConstraint(GameObject* object, const float heightValue);
			~FixedHeightConstraint();

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* object;

			float height;
		};
	}
}