#pragma once
#include "PushdownState.h"
#include "../GameTech/NetworkedGame.h"

using namespace NCL::CSC8503;

class ServerState : public PushdownState
{
public:
	ServerState() {}
	~ServerState() { delete game; }

	void OnAwake(float dt);
	void OnSleep(float dt) {}

	PushdownResult PushdownUpdate(PushdownState** pushResult);
protected:
	NetworkedGame* game = new NetworkedGame();
};