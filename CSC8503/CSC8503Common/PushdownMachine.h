#pragma once
#include <stack>

namespace NCL {
	namespace CSC8503 {
		class PushdownState;

		class PushdownMachine
		{
		public:
			PushdownMachine();
			~PushdownMachine();

			void Update(float dt);
			bool IsEmpty() { return stateStack.empty(); }

		protected:
			PushdownState* activeState;

			std::stack<PushdownState*> stateStack;
		};
	}
}

