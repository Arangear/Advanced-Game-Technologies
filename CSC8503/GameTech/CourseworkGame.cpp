#include "CourseworkGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"
#include "../../CSC8503/CSC8503Common/PickableObject.h"

#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/HingeConstraint.h"

using namespace NCL;
using namespace CSC8503;

CourseworkGame::CourseworkGame()
{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 300.0f;

	Debug::SetRenderer(renderer);

	InitialiseAssets();
}

void CourseworkGame::InitialiseAssets()
{
	auto loadFunc = [](const string& name, OGLMesh** into)
	{
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("goose.msh", &gooseMesh);
	loadFunc("CharacterA.msh", &keeperMesh);
	loadFunc("CharacterM.msh", &charA);
	loadFunc("CharacterF.msh", &charB);
	loadFunc("Apple.msh", &appleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
	InitWorld();
}

void CourseworkGame::DisplayGrid()
{
	NavigationGrid grid("2DMap.txt");

	for (int i = 0; i < grid.GetNodeCount(); i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (grid.GetNodes()[i].connected[j])
			{
				renderer->DrawLine(grid.GetNodes()[i].position + Vector3(0, 1, 0), grid.GetNodes()[i].connected[j]->position + Vector3(0, 1, 0), Vector4(0, 0.2, 0.8, 1));
			}
		}
	}
}

CourseworkGame::~CourseworkGame()
{
	delete cubeMesh;
	delete sphereMesh;
	delete gooseMesh;
	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

void CourseworkGame::UpdateGame(float dt)
{
	EndGame(dt);

	playerCharacter->UpdatePositions();
	playerCharacter->ManageAbilities(dt);

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

void CourseworkGame::UpdateKeys(float dt)
{
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
	{
		InitWorld(); //We can reset the simulation at any time with F1
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9))
	{
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10))
	{
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7))
	{
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8))
	{
		world->ShuffleObjects(false);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::L))
	{
		toggleGrid = !toggleGrid;
	}
}

void  CourseworkGame::CameraMovement()
{
	Vector3 objPos = playerCharacter->GetTransform().GetWorldPosition();
	Vector3 camPos = objPos + playerCharacter->GetConstTransform().GetLocalOrientation() * lockedOffset;

	Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos + Vector3(0, 5, 0), Vector3(0, 1, 0));

	Matrix4 modelMat = temp.Inverse();

	Quaternion q(modelMat);
	Vector3 angles = q.ToEuler(); //nearly there now!

	world->GetMainCamera()->SetPosition(camPos);
	world->GetMainCamera()->SetPitch(angles.x);
	world->GetMainCamera()->SetYaw(angles.y);
}

void CourseworkGame::MovePlayerCharacter(float dt)
{
	float rotationSpeed = 60.0f;
	Vector3 pyr = playerCharacter->GetConstTransform().GetLocalOrientation().ToEuler();

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A))
	{
		pyr.y += rotationSpeed * dt;
		pyr.y = pyr.y >= 0.0f ? pyr.y <= 360.0f ? pyr.y : pyr.y - 360.0f : pyr.y + 360.0f;
		playerCharacter->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(pyr.x, pyr.y, pyr.z));
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W))
	{
		playerCharacter->GetPhysicsObject()->AddForce(playerCharacter->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, 1) * forceMagnitude * playerCharacter->GetSpeedMultiplier());
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S))
	{
		playerCharacter->GetPhysicsObject()->AddForce(playerCharacter->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, -1) * forceMagnitude * playerCharacter->GetSpeedMultiplier());
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::D))
	{
		pyr.y -= rotationSpeed * dt;
		pyr.y = pyr.y >= 0.0f ? pyr.y <= 360.0f ? pyr.y : pyr.y - 360.0f : pyr.y + 360.0f;
		playerCharacter->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(pyr.x, pyr.y, pyr.z));
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SPACE))
	{
		playerCharacter->GetPhysicsObject()->AddForce(Vector3(0, 2, 0) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::Q))
	{
		if (playerCharacter->GetSprintCD() <= 0)
		{
			playerCharacter->GetSprint() = 3.0f;
			playerCharacter->GetSprintCD() = 5.0f;
			playerCharacter->GetSpeedMultiplier() = 2.0f;
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::E))
	{
		if (playerCharacter->GetQuackAttackCD() <= 0)
		{
			playerCharacter->GetQuackAttackCD() = 5.0f;
			playerCharacter->GetQuackID()++;
			playerCharacter->GetQuackID() = playerCharacter->GetQuackID() >= 6 ? 0 : playerCharacter->GetQuackID();
			QuackAttack();
		}
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::SHIFT))
	{
		playerCharacter->GetPhysicsObject()->AddForce(Vector3(0, -2, 0) * forceMagnitude);
	}
}

void CourseworkGame::MoveEnemies(float dt)
{
	for (int i = 0; i < enemies.size(); i++)
	{
		enemies[i]->UpdatePosition(playerCharacter, dt);
	}
}

void CourseworkGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(2000.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
}

void CourseworkGame::InitWorld()
{
	world->ClearAndErase();
	physics->Clear();
	playerCharacter->GetSprint() = 0.0f;
	playerCharacter->GetSprintCD() = 0.0f;
	playerCharacter->GetQuackAttackCD() = 0.0f;
	playerCharacter->GetQuackID() = 0.0f;
	playerCharacter->GetSpeedMultiplier() = 0.0f;
	forceMagnitude = 300.0f;
	timer = 180.0f;
	pickables.clear();
	enemies.clear();
	obstacles.clear();

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

void CourseworkGame::QuackAttack()
{
	for (GameObject*& object : movables)
	{
		Vector3 direction = object->GetConstTransform().GetWorldPosition() - playerCharacter->GetConstTransform().GetWorldPosition();
		if (direction.Length() < playerCharacter->GetQuackRange())
		{
			direction.Normalise();

			Ray* ray = new Ray(playerCharacter->GetConstTransform().GetWorldPosition(), direction);
			RayCollision collision;
			if (world->Raycast(*ray, collision, true))
			{
				object->GetPhysicsObject()->AddForceAtPosition(direction * playerCharacter->GetQuackForce(), collision.collidedAt);
			}
			delete ray;
		}
	}
	for (EnemyObject*& enemy : enemies)
	{
		GameObject* object = (GameObject*&)enemy;
		Vector3 direction = object->GetConstTransform().GetWorldPosition() - playerCharacter->GetConstTransform().GetWorldPosition();
		if (direction.Length() < playerCharacter->GetQuackRange())
		{
			direction.Normalise();

			Ray* ray = new Ray(playerCharacter->GetConstTransform().GetWorldPosition(), direction);
			RayCollision collision;
			if (world->Raycast(*ray, collision, true))
			{
				object->GetPhysicsObject()->AddForceAtPosition(direction * 3 * playerCharacter->GetQuackForce(), collision.collidedAt);
			}
			delete ray;
		}
	}
}

void CourseworkGame::EndGame(float dt)
{
	std::set<PickableObject*> toRemove;
	//Find objects to remove in this frame
	for (PickableObject* object : pickables)
	{
		if (object->GetAssigned())
		{
			toRemove.insert(object);
		}
	}
	//Remove the objects from the set
	for (PickableObject* object : toRemove)
	{
		pickables.erase(object);
	}
	if (pickables.size() == 0 || timer <= 0)//All objects placed on islands or 3 minutes have passed
	{
		gameFinished = true;
	}

	timer -= dt;
}

//From here on it's functions to add in objects to the world!

GameObject* CourseworkGame::AddFloorToWorld(const Vector3& position, const Vector3& scale, const Vector4& colour, const int collisionResolution)
{
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(scale);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetWorldScale(scale);
	floor->GetTransform().SetWorldPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));
	floor->GetPhysicsObject()->SetElasticity(0.0f);
	floor->GetPhysicsObject()->SetBuoyancy(0.0f);
	floor->GetPhysicsObject()->SetGravityAffinity(false);
	floor->GetPhysicsObject()->SetCollisionResolution(collisionResolution);

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->GetRenderObject()->SetColour(colour);
	floor->GetRenderObject()->SetOriginalColour(colour);

	world->AddGameObject(floor);

	return floor;
}

GameObject* CourseworkGame::AddRampToWorld(const Vector3& position, const Vector3& scale, const Vector3& rotation, const float inverseMass)
{
	GameObject* ramp = new GameObject();

	OBBVolume* volume = new OBBVolume(scale);
	ramp->SetBoundingVolume((CollisionVolume*)volume);
	ramp->GetTransform().SetWorldScale(scale);
	ramp->GetTransform().SetWorldPosition(position);
	ramp->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z));

	ramp->SetRenderObject(new RenderObject(&ramp->GetTransform(), cubeMesh, basicTex, basicShader));
	ramp->SetPhysicsObject(new PhysicsObject(&ramp->GetTransform(), ramp->GetBoundingVolume()));
	ramp->GetPhysicsObject()->SetElasticity(0.0f);
	ramp->GetPhysicsObject()->SetBuoyancy(0.0f);
	ramp->GetPhysicsObject()->SetGravityAffinity(false);
	ramp->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse);

	ramp->GetPhysicsObject()->SetInverseMass(inverseMass);
	ramp->GetPhysicsObject()->InitCubeInertia();

	ramp->GetRenderObject()->SetColour(Vector4(1, 0.89, 0.77, 1));
	ramp->GetRenderObject()->SetOriginalColour(Vector4(1, 0.89, 0.77, 1));

	world->AddGameObject(ramp);

	return ramp;
}

GameObject* CourseworkGame::AddIslandToWorld(const Vector3& position, const Vector3& scale, const Vector4& colour, const int collisionResolution)
{
	GooselandObject* island = new GooselandObject();

	AABBVolume* volume = new AABBVolume(scale);
	island->SetBoundingVolume((CollisionVolume*)volume);
	island->GetTransform().SetWorldScale(scale);
	island->GetTransform().SetWorldPosition(position);

	island->SetRenderObject(new RenderObject(&island->GetTransform(), cubeMesh, basicTex, basicShader));
	island->SetPhysicsObject(new PhysicsObject(&island->GetTransform(), island->GetBoundingVolume()));
	island->GetPhysicsObject()->SetElasticity(0.0f);
	island->GetPhysicsObject()->SetBuoyancy(0.0f);
	island->GetPhysicsObject()->SetGravityAffinity(false);
	island->GetPhysicsObject()->SetCollisionResolution(collisionResolution);

	island->GetPhysicsObject()->SetInverseMass(0);
	island->GetPhysicsObject()->InitCubeInertia();
	island->SetOwner(playerCharacter);

	island->GetRenderObject()->SetColour(colour);
	island->GetRenderObject()->SetOriginalColour(colour);

	world->AddGameObject(island);

	return island;
}

GameObject* CourseworkGame::AddWallToWorld(const Vector3& position, const Vector3& scale)
{
	GameObject* wall = new GameObject();

	AABBVolume* volume = new AABBVolume(scale);
	wall->SetBoundingVolume((CollisionVolume*)volume);
	wall->GetTransform().SetWorldScale(scale);
	wall->GetTransform().SetWorldPosition(position);

	wall->SetRenderObject(new RenderObject(&wall->GetTransform(), cubeMesh, basicTex, basicShader));
	wall->SetPhysicsObject(new PhysicsObject(&wall->GetTransform(), wall->GetBoundingVolume()));
	wall->GetPhysicsObject()->SetElasticity(0.0f);
	wall->GetPhysicsObject()->SetBuoyancy(0.0f);
	wall->GetPhysicsObject()->SetGravityAffinity(false);
	wall->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse);

	wall->GetPhysicsObject()->SetInverseMass(0);
	wall->GetPhysicsObject()->InitCubeInertia();

	wall->GetRenderObject()->SetColour(Vector4(0.5, 0.5, 0.5, 1));
	wall->GetRenderObject()->SetOriginalColour(Vector4(0.5, 0.5, 0.5, 1));

	world->AddGameObject(wall);

	return wall;
}

void CourseworkGame::AddTrampolineToWorld(const Vector3& position)
{
	AddFloorToWorld(position, Vector3(25, 1.5, 25), Vector4(1, 0, 0, 1), CollisionResolution::Trampoline);
	obstacles.push_back(AddWallToWorld(position + Vector3(25, 0, 4.7), Vector3(4.7, 2.5, 25)));
	obstacles.push_back(AddWallToWorld(position - Vector3(25, 0, 4.7), Vector3(4.7, 2.5, 25)));
	obstacles.push_back(AddWallToWorld(position + Vector3(-4.7, 0, 25), Vector3(25, 2.5, 4.7)));
	obstacles.push_back(AddWallToWorld(position - Vector3(-4.7, 0, 25), Vector3(25, 2.5, 4.7)));
}

GameObject* CourseworkGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	PickableObject* sphere = new PickableObject(5);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);

	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetPhysicsObject()->SetElasticity(0.9f);
	sphere->GetPhysicsObject()->SetBuoyancy(100.0f);
	sphere->GetPhysicsObject()->SetGravityAffinity(false);
	sphere->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Collect);

	Vector4 colour = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
	sphere->GetRenderObject()->SetColour(colour);
	sphere->GetRenderObject()->SetOriginalColour(colour);

	sphere->SetOrigin(position);

	world->AddGameObject(sphere);
	pickables.insert(sphere);

	return sphere;
}

GameObject* CourseworkGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetWorldPosition(position);
	cube->GetTransform().SetWorldScale(dimensions);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));
	cube->GetPhysicsObject()->SetElasticity((rand() % 100 + 1) / 100.f);
	cube->GetPhysicsObject()->SetBuoyancy(rand() % 351 + 50);
	cube->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse | CollisionResolution::Spring | CollisionResolution::Trampoline);

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GooseObject* CourseworkGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass	= 0.25f;

	GooseObject* goose = new GooseObject();

	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->GetPhysicsObject()->SetBuoyancy(140);
	goose->GetPhysicsObject()->SetElasticity(0.7f);
	goose->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse | CollisionResolution::Spring | CollisionResolution::Trampoline | CollisionResolution::Collect);

	world->AddGameObject(goose);

	return goose;
}

GameObject* CourseworkGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	EnemyObject* keeper = new EnemyObject("2DMap.txt");

	keeper->SetOrigin(position);
	keeper->AddObstacles(obstacles);

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();
	keeper->GetPhysicsObject()->SetBuoyancy(70);
	keeper->GetPhysicsObject()->SetElasticity(0.7f);
	keeper->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse | CollisionResolution::Spring | CollisionResolution::Trampoline);

	world->AddGameObject(keeper);
	enemies.push_back(keeper);

	return keeper;
}

GameObject* CourseworkGame::AddCharacterToWorld(const Vector3& position) {
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	auto pos = keeperMesh->GetPositionData();

	Vector3 minVal = pos[0];
	Vector3 maxVal = pos[0];

	for (auto& i : pos) {
		maxVal.y = max(maxVal.y, i.y);
		minVal.y = min(minVal.y, i.y);
	}

	GameObject* character = new GameObject();

	float r = rand() / (float)RAND_MAX;


	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	character->GetTransform().SetWorldPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), r > 0.5f ? charA : charB, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(character);

	return character;
}

void CourseworkGame::AddGateToWorld(const Vector3& position, const Vector3& rotation)
{
	GameObject* fence1 = AddRampToWorld(position, Vector3(0.2, 2, 0.3), rotation, 0);
	GameObject* gate = AddRampToWorld(position + Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z) * Vector3(12, 0, 0), Vector3(11.5, 1.8, 0.2), rotation, 0.5);
	GameObject* fence2 = AddRampToWorld(position + Quaternion::EulerAnglesToQuaternion(rotation.x, rotation.y, rotation.z) * Vector3(24, 0, 0), Vector3(0.2, 2, 0.3), rotation, 0);

	PositionConstraint* constraint = new PositionConstraint(fence1, gate, 12);
	HingeConstraint* rot = new HingeConstraint(fence1, gate, Vector3(0, 1, 0), Quaternion::EulerAnglesToQuaternion(-rotation.x, -rotation.y, -rotation.z) * Vector3(-12, 0, 0));

	world->AddConstraint(constraint);
	world->AddConstraint(rot);
}

GameObject* CourseworkGame::AddAppleToWorld(const Vector3& position)
{
	PickableObject* apple = new PickableObject(1);

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(4, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetPhysicsObject()->SetBuoyancy(50);
	apple->GetPhysicsObject()->SetElasticity(0.7f);
	apple->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Collect);
	apple->GetPhysicsObject()->SetGravityAffinity(false);

	apple->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	apple->GetRenderObject()->SetOriginalColour(Vector4(1, 0, 0, 1));
	apple->SetOrigin(position);

	world->AddGameObject(apple);
	pickables.insert(apple);

	return apple;
}

void CourseworkGame::DrawDisplay(float dt)
{
	renderer->DrawString("Items held: " + std::to_string(playerCharacter->GetItemCount()), Vector2(10, 10));
	renderer->DrawString("Points: " + std::to_string(playerCharacter->GetPoints()), Vector2(10, 30));
	renderer->DrawString("Items remaining: " + std::to_string(pickables.size()), Vector2(10, 50));
	renderer->DrawString("Game over in: " + std::to_string(timer), Vector2(10, 70));
	if (playerCharacter->GetQuackAttackCD() > 0.0f)
	{
		renderer->DrawString("Quack cd: " + std::to_string(playerCharacter->GetQuackAttackCD()), Vector2(10, 90));
	}
	if (playerCharacter->GetSprintCD() > 0.0f)
	{
		renderer->DrawString("Sprint cd: " + std::to_string(playerCharacter->GetSprintCD()), Vector2(10, 110));
	}
	if (playerCharacter->GetSprint() > 0.0f)
	{
		renderer->DrawString("Sprint over in: " + std::to_string(playerCharacter->GetSprint()), Vector2(10, 130));
	}
	//Fun purposes
	if (playerCharacter->GetQuackAttackCD() > 4.0f)
	{
		std::pair<string, std::pair<int, int>> message = playerCharacter->GetMessages()[playerCharacter->GetQuackID()];
		renderer->DrawString(message.first, Vector2(message.second.first, message.second.second), Vector4(0, 0.88, 0.72, 1));
	}
}

