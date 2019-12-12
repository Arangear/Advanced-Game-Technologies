#pragma once
#include "GameTechRenderer.h"

using namespace NCL;
using namespace CSC8503;

enum MenuOption {Start, Host, Join, Quit};

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

	vector<string> buttons = { "Start singleplayer game", "Set up server", "Join a hosted game", "Quit" };
	int selected = 0;
};