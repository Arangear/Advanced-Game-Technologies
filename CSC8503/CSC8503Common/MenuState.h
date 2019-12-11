#pragma once
#include "PushdownState.h"
#include "../GameTech/MenuLevel.h"

using namespace NCL;
using namespace CSC8503;

class MenuState : public PushdownState
{
public:
	MenuState() {}
	~MenuState() { delete menu; }

	void OnAwake(float dt);
	void OnSleep(float dt) {}

	PushdownResult PushdownUpdate(PushdownState** pushResult);
protected:
	MenuLevel* menu = new MenuLevel();
};