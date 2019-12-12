#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"
#include "CourseworkPacketReceiver.h"

#define COLLISION_MSG 30

NetworkedGame::NetworkedGame() : CourseworkGame()
{

}

NetworkedGame::~NetworkedGame()
{
	NetworkBase::Destroy();
}

void NetworkedGame::StartAsServer()
{
	NetworkBase::Initialise();
	CourseworkPacketReceiver serverReceiver("Server");

	int port = NetworkBase::GetDefaultPort();

	thisServer = new GameServer(port, 1);
	thisClient = nullptr;

	thisServer->RegisterPacketHandler(String_Message, &serverReceiver);
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d)
{
	NetworkBase::Initialise();
	CourseworkPacketReceiver clientReceiver("Client");

	int port = NetworkBase::GetDefaultPort();

	thisServer = nullptr;
	thisClient = new GameClient();

	thisClient->RegisterPacketHandler(String_Message, &clientReceiver);

	thisClient->Connect(a, b, c, d, port);
}

void NetworkedGame::UpdateGame(float dt)
{
	EndGame(dt);

	for (std::pair<const int, GooseObject*>& player : serverPlayers)
	{
		player.second->UpdatePositions();
		player.second->ManageAbilities(dt);
	}


	CameraMovement();

	UpdateKeys(dt);

	MovePlayerCharacter(dt);
	MoveEnemies(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();

	//Display AI pathfinding grid
	if (toggleGrid)
	{
		DisplayGrid();
	}

	DrawDisplay(dt);
}

