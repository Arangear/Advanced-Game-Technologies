#pragma once
#include "State.h"

namespace NCL {
	namespace CSC8503 {
		class PushdownState : public State
		{
		public:
			enum PushdownResult {
				Push, Pop, NoChange
			};
			PushdownState();
			~PushdownState();

			virtual PushdownResult PushdownUpdate(PushdownState** pushResult);

			virtual void Update() {}
			virtual void OnAwake(float dt) {} //By default do nothing
			virtual void OnSleep(float dt) {} //By default do nothing
		};
	}
}

