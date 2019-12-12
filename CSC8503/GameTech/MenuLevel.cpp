#include "MenuLevel.h"

MenuLevel::MenuLevel()
{
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);

	DrawMenu();
}

MenuLevel::~MenuLevel()
{
	delete renderer;
	delete world;
}

void MenuLevel::UpdateMenu(float dt)
{
	Debug::SetRenderer(renderer);
	world->UpdateWorld(dt);
	renderer->Update(dt);

	renderer->Render();

	DrawMenu();

	SetKeys();
}

void MenuLevel::DrawMenu()
{
	for (int i = 0; i < buttons.size(); i++)
	{
		renderer->DrawString(buttons[i], Vector2(10, (10 + 20 * buttons.size()) - 20 * i), i == selected ? Vector4(0.3, 0.3, 0, 1) : Vector4(0, 0, 0, 1));
	}
}

void MenuLevel::SetKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S))
	{
		selected++;
		while (selected >= buttons.size())
		{
			selected -= buttons.size();
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W))
	{
		selected--;
		while (selected < 0)
		{
			selected += buttons.size();
		}
	}
}