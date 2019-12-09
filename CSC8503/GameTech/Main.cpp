#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "CourseworkGame.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

vector<Vector3> testNodes;

void TestPathfinding()
{
	NavigationGrid grid("2DMap.txt");
	
	NavigationPath outPath;
	
	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);
	
	bool found = grid.FindPath(startPos, endPos, outPath);
	
	Vector3 pos;
	while (outPath.PopWaypoint(pos))
	{
		testNodes.push_back(pos);
	}
}

void DisplayPathfinding()
{
	for (int i = 1; i < testNodes.size(); ++i)
	{
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a+ Vector3(0,15,0), b + Vector3(0, 15, 0), Vector4(1, 1, 0, 1));
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

	if (!w->HasInitialised()) {
		return -1;
	}	

	TestPathfinding();
	
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	CourseworkGame* g = new CourseworkGame();

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();

		if (dt > 1.0f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		w->SetTitle("FPS:" + std::to_string(1.0f / dt));

		g->UpdateGame(dt);

		DisplayPathfinding();
	}
	Window::DestroyGameWindow();
}