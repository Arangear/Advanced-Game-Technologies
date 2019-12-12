#pragma once
#include "PushdownState.h"
#include "../GameTech/NetworkedGame.h"

using namespace NCL::CSC8503;

class PlayerState : public PushdownState
{
public:
	PlayerState() {}
	~PlayerState() { delete game; }

	void OnAwake(float dt);
	void OnSleep(float dt) {}

	PushdownResult PushdownUpdate(PushdownState** pushResult);
protected:
	NetworkedGame* game = new NetworkedGame();
};