#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"

#include "CourseworkGame.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

bool toggleGrid = false;

void DisplayGrid()
{
	NavigationGrid grid("2DMap.txt");

	for (int i = 0; i < grid.GetNodeCount(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (grid.GetNodes()[i].connected[j])
			{
				Debug::DrawLine(grid.GetNodes()[i].position + Vector3(0,1,0), grid.GetNodes()[i].connected[j]->position + Vector3(0, 1, 0), Vector4(1, 0, 0, 1));
			}
		}
	}
}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/
int main()
{
	Window*w = Window::CreateGameWindow("CSC8503 Advanced Game Technologies", 1280, 720);

	if (!w->HasInitialised())
	{
		return -1;
	}
	
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	CourseworkGame* g = new CourseworkGame();

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE))
	{
		float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 1.0f)
		{
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR))
		{
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT))
		{
			w->ShowConsole(false);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L))
		{
			toggleGrid = !toggleGrid;
		}


		//Display AI pathfinding grid
		if (toggleGrid)
		{
			DisplayGrid();
		}

		w->SetTitle("FPS:" + std::to_string(1.0f / dt));

		g->UpdateGame(dt);

		Debug::FlushRenderables();
	}
	Window::DestroyGameWindow();
}