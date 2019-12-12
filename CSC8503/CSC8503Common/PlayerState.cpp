#include "PlayerState.h"

void PlayerState::OnAwake(float dt)
{
	//game->UpdateGame(dt);
}

PushdownState::PushdownResult PlayerState::PushdownUpdate(PushdownState** pushResult)
{
	if (game->IsOver() || Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
	{
		return PushdownState::PushdownResult::Pop;
	}
	return PushdownState::PushdownResult::NoChange;
}