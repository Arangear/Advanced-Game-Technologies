#include "MenuState.h"
#include "GameState.h"
#include "ServerState.h"
#include "PlayerState.h"

void MenuState::OnAwake(float dt)
{
	menu->UpdateMenu(dt);
}

PushdownState::PushdownResult MenuState::PushdownUpdate(PushdownState** pushResult)
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RETURN))
	{
		switch (menu->GetSelected())
		{
		case MenuOption::Start:
		{
			*pushResult = new GameState();
			return PushdownState::PushdownResult::Push;
		}
		case MenuOption::Host:
		{
			*pushResult = new ServerState();
			return PushdownState::PushdownResult::Push;
		}
		case MenuOption::Join:
		{
			*pushResult = new PlayerState();
			return PushdownState::PushdownResult::Push;
		}
		case MenuOption::Quit:
		{
			return PushdownState::PushdownResult::Pop;
		}
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
	{
		return PushdownState::PushdownResult::Pop;
	}
	return PushdownState::PushdownResult::NoChange;
}