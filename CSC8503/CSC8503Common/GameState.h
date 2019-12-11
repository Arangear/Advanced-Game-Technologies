#pragma once
#include "PushdownState.h"
#include "../GameTech/CourseworkGame.h"

using namespace NCL;
using namespace CSC8503;

class GameState : public PushdownState
{
public:
	GameState() {}
	~GameState() { delete game; }

	void OnAwake(float dt);
	void OnSleep(float dt) {}

	PushdownResult PushdownUpdate(PushdownState** pushResult);
protected:
	CourseworkGame* game = new CourseworkGame();
};