#include "ServerState.h"

void ServerState::OnAwake(float dt)
{
	//game->UpdateGame(dt);
}

PushdownState::PushdownResult ServerState::PushdownUpdate(PushdownState** pushResult)
{
	if (game->IsOver() || Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
	{
		return PushdownState::PushdownResult::Pop;
	}
	return PushdownState::PushdownResult::NoChange;
}