#include "PushdownMachine.h"
#include "PushdownState.h"
#include "GameState.h"
#include "MenuState.h"
using namespace NCL::CSC8503;

PushdownMachine::PushdownMachine()
{
	activeState = nullptr;
}

PushdownMachine::~PushdownMachine()
{
}

void PushdownMachine::Update(float dt) {
	if (activeState) {
		PushdownState* newState = nullptr;
		PushdownState::PushdownResult result = activeState->PushdownUpdate(&newState);

		switch (result) {
		case PushdownState::Pop: {
			activeState->OnSleep(dt);
			stateStack.pop();
			if (stateStack.empty())
			{//Exit the game
				activeState = nullptr; //??????
				exit(0);
			}
			else {
				activeState = stateStack.top();
				activeState->OnAwake(dt);
			}
			break;
		}
		case PushdownState::Push: {
			activeState->OnSleep(dt);
			stateStack.push(newState);
			activeState = stateStack.top();
			newState->OnAwake(dt);
			break;
		}
		case PushdownState::NoChange:
		{
			activeState->OnAwake(dt);
			break;
		}
		}
	}
	else
	{ //Nothin on stack, initialise
		MenuState* menuState = new MenuState();
		stateStack.push(menuState);
		activeState = stateStack.top();
		activeState->OnAwake(dt);
	}
}