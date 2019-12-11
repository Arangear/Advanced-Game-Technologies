#pragma once
#include "GameTechRenderer.h"

using namespace NCL;
using namespace CSC8503;

class MenuLevel
{
public:
	MenuLevel();
	~MenuLevel();

	virtual void UpdateMenu(float dt);

protected:
	void DrawMenu();

	GameTechRenderer* renderer;
	GameWorld* world;
};