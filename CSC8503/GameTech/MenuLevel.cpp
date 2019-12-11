#include "MenuLevel.h"

MenuLevel::MenuLevel()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);

	Debug::SetRenderer(renderer);

	DrawMenu();
}

MenuLevel::~MenuLevel()
{
	delete renderer;
	delete world;
}

void MenuLevel::UpdateMenu(float dt)
{
	world->UpdateWorld(dt);
	renderer->Update(dt);

	renderer->Render();

	DrawMenu();
}