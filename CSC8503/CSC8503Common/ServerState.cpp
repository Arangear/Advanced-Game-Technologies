#include "ServerState.h"

void ServerState::OnAwake(float dt)
{
	game->UpdateGame(dt);
}

PushdownState::PushdownResult ServerState::PushdownUpdate(PushdownState** pushResult)
{
	if (game->IsOver())
	{
		return PushdownState::PushdownResult::Pop;
	}
	return PushdownState::PushdownResult::NoChange;
}