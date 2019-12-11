#include "GameState.h"

void GameState::OnAwake(float dt)
{
	game->UpdateGame(dt);
}

PushdownState::PushdownResult GameState::PushdownUpdate(PushdownState** pushResult)
{
	if (game->IsOver() || Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
	{
		return PushdownState::PushdownResult::Pop;
	}
	return PushdownState::PushdownResult::NoChange;
}