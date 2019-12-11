#pragma once
#include "GameTechRenderer.h"

using namespace NCL;
using namespace CSC8503;

enum MenuOption {Start, Quit};

class MenuLevel
{
public:
	MenuLevel();
	~MenuLevel();

	virtual void UpdateMenu(float dt);

	MenuOption GetSelected() { return (MenuOption)selected; }

protected:
	void DrawMenu();
	void SetKeys();

	GameTechRenderer* renderer;
	GameWorld* world;

	vector<string> buttons = { "Start game", "Quit" };
	int selected = 0;
};