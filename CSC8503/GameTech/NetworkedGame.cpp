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

	//TODO
	//Wait for a player to connect
	while (true)
	{
		//do something
	}

	StartLevel();
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
	MoveEnemies(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);
}

void NetworkedGame::SpawnPlayer(int id)
{
	//Spawn player and attach him to his island and controller
	GooseObject* player = new GooseObject();

}

void NetworkedGame::StartLevel()
{
	world->ClearAndErase();
	physics->Clear();
	forceMagnitude = 300.0f;
	timer = 180.0f;
	pickables.clear();
	enemies.clear();
	obstacles.clear();

	//Spawn players
	for (int i = 0; i < 2; i++)
	{
		SpawnPlayer(i);
	}

	//Setup
	Vector3 floorPosition(0, -10, 0);
	Vector3 floorSize(250, 10, 250);
	Vector3 wallSizeX(250, 250, 2);
	Vector3 wallSizeZ(2, 250, 250);
	Vector3 islandSize(25, 2, 25);
	Vector4 floorColour(0, 0, 1, 1);
	Vector4 islandColour(1, 1, 0, 1);
	Vector4 terrainColour(0, 1, 0, 1);
	//Goose
	playerCharacter = AddGooseToWorld(Vector3(212.5, 4, 212.5));
	//Cage
	AddFloorToWorld(floorPosition, floorSize, floorColour, CollisionResolution::Spring);
	AddFloorToWorld(floorPosition, floorSize - Vector3(0, 2, 0), floorColour, CollisionResolution::Impulse);
	obstacles.push_back(AddWallToWorld(floorPosition + Vector3(floorSize.x, floorSize.x, 0.0f), wallSizeZ));
	obstacles.push_back(AddWallToWorld(floorPosition - Vector3(floorSize.x, -floorSize.x, 0.0f), wallSizeZ));
	obstacles.push_back(AddWallToWorld(floorPosition + Vector3(0.0f, floorSize.z, floorSize.z), wallSizeX));
	obstacles.push_back(AddWallToWorld(floorPosition - Vector3(0.0f, -floorSize.z, floorSize.z), wallSizeX));
	//Islands
	AddIslandToWorld(floorPosition + floorSize * 0.85f, islandSize, islandColour, CollisionResolution::Impulse);
	AddIslandToWorld(floorPosition - Vector3(floorSize.x, -floorSize.y, floorSize.z) * 0.85f, islandSize, islandColour, CollisionResolution::Impulse);
	AddGateToWorld(floorPosition + floorSize * 0.85f + Vector3(-12, 2, -12), Vector3(0, 0, 0));
	//Terrain
	AddFloorToWorld(Vector3(0, -1, 0), Vector3(250, 1.5, 150), terrainColour, CollisionResolution::Impulse);
	obstacles.push_back(AddFloorToWorld(Vector3(200.15, 2.5, -100.15), Vector3(49.85, 2.5, 49.85), terrainColour, CollisionResolution::Impulse));
	AddFloorToWorld(Vector3(210, 7.5, -110), Vector3(40, 2.5, 40), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(220, 12.5, -120), Vector3(30, 2.5, 30), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(230, 17.5, -130), Vector3(20, 2.5, 20), terrainColour, CollisionResolution::Impulse);
	obstacles.push_back(AddFloorToWorld(Vector3(-200.15, 2.5, 100.15), Vector3(49.85, 2.5, 49.85), terrainColour, CollisionResolution::Impulse));
	AddFloorToWorld(Vector3(-210, 7.5, 110), Vector3(40, 2.5, 40), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-220, 12.5, 120), Vector3(30, 2.5, 30), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-230, 17.5, 130), Vector3(20, 2.5, 20), terrainColour, CollisionResolution::Impulse);
	//Ramps
	AddRampToWorld(Vector3(-230, 40, 130), Vector3(10, 1, 10), Vector3(-20, 0, -20), 0);
	AddRampToWorld(Vector3(230, 40, -130), Vector3(10, 1, 10), Vector3(20, 0, 20), 0);
	//Trampoline
	AddTrampolineToWorld(Vector3(0, 2.5, 0));
	//Apples
	AddAppleToWorld(Vector3(0, 30, 0));
	AddAppleToWorld(Vector3(-20, 30, -20));
	AddAppleToWorld(Vector3(-20, 30, 20));
	AddAppleToWorld(Vector3(20, 30, -20));
	AddAppleToWorld(Vector3(20, 30, 20));
	AddAppleToWorld(Vector3(230, 22.5, -130));
	AddAppleToWorld(Vector3(240, 22.5, -140));
	AddAppleToWorld(Vector3(240, 22.5, -120));
	AddAppleToWorld(Vector3(220, 22.5, -140));
	AddAppleToWorld(Vector3(220, 22.5, -120));
	AddAppleToWorld(Vector3(-230, 22.5, 130));
	AddAppleToWorld(Vector3(-240, 22.5, 140));
	AddAppleToWorld(Vector3(-240, 22.5, 120));
	AddAppleToWorld(Vector3(-220, 22.5, 140));
	AddAppleToWorld(Vector3(-220, 22.5, 120));
	//Obstacles
	AddCubeToWorld(Vector3(0, 20, 0), Vector3(3, 3, 3), 0.14f);
	obstacles.push_back(AddCubeToWorld(Vector3(110, 1.5, -12.5), Vector3(4.7, 3, 37.2), 0.0f));
	obstacles.push_back(AddCubeToWorld(Vector3(85.3, 1.5, -45), Vector3(20, 3, 4.85), 0.0f));
	obstacles.push_back(AddCubeToWorld(Vector3(-110, 1.5, 12.5), Vector3(4.7, 3, 37.2), 0.0f));
	obstacles.push_back(AddCubeToWorld(Vector3(-85.3, 1.5, 45), Vector3(20, 3, 4.85), 0.0f));
	movables.push_back(AddCubeToWorld(Vector3(120, 1, 120), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 1, 125), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 1, 130), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 1, 135), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 1, 140), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 5, 122.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 5, 127.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 5, 132.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 5, 137.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 9, 125), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 9, 130), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 9, 135), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 1, -120), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 1, -125), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 1, -130), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 1, -135), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 1, -140), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 5, -122.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 5, -127.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 5, -132.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 5, -137.5), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 9, -125), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 9, -130), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 9, -135), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(120, 2, 190), Vector3(2, 2, 2), 0.5f));
	movables.push_back(AddCubeToWorld(Vector3(-120, 2, -190), Vector3(2, 2, 2), 0.5f));
	//Enemies
	AddParkKeeperToWorld(Vector3(-150, 10, -12.5));
	AddParkKeeperToWorld(Vector3(150, 10, 12.5));
	//Other collectibles
	AddSphereToWorld(Vector3(135, 2, 95), 1, 1);
	AddSphereToWorld(Vector3(-135, 2, -95), 1, 1);
	AddSphereToWorld(Vector3(-230, 42, 130), 1, 1);
	AddSphereToWorld(Vector3(230, 42, -130), 1, 1);
}

void NetworkedGame::MoveEnemies(float dt)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		//TODO: get enemies to target players somehow
		enemies[i]->UpdatePosition(serverPlayers[i % 2], dt);
	}
}

