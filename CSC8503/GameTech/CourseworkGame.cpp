#include "CourseworkGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include "../CSC8503Common/PositionConstraint.h"

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
	CameraMovement();

	UpdateKeys(dt);

	MovePlayerCharacter(dt);

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
}

void CourseworkGame::UpdateKeys(float dt)
{
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
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
	float rotationSpeed = 40.0f;
	Vector3 pyr = playerCharacter->GetConstTransform().GetLocalOrientation().ToEuler();

	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::A))
	{
		pyr.y += rotationSpeed * dt;
		pyr.y = pyr.y >= 0.0f ? pyr.y <= 360.0f ? pyr.y : pyr.y - 360.0f : pyr.y + 360.0f;
		playerCharacter->GetTransform().SetLocalOrientation(Quaternion::EulerAnglesToQuaternion(pyr.x, pyr.y, pyr.z));
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::W))
	{
		playerCharacter->GetPhysicsObject()->AddForce(playerCharacter->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, 1) * forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(NCL::KeyboardKeys::S))
	{
		playerCharacter->GetPhysicsObject()->AddForce(playerCharacter->GetConstTransform().GetLocalOrientation() * Vector3(0, 0, -1) * forceMagnitude);
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

	//Setup
	Vector3 floorPosition(0, -2, 0);
	Vector3 floorSize(500, 2, 500);
	Vector3 wallSizeX(500, 500, 2);
	Vector3 wallSizeZ(2, 500, 500);
	Vector3 islandSize(50, 2, 50);
	Vector4 floorColour(0, 0, 1, 1);
	Vector4 islandColour(1, 1, 0, 1);
	Vector4 terrainColour(0, 1, 0, 1);
	//Cage
	AddFloorToWorld(floorPosition, floorSize, floorColour, CollisionResolution::Spring);
	AddWallToWorld(floorPosition + Vector3(floorSize.x, floorSize.x, 0.0f), wallSizeZ);
	AddWallToWorld(floorPosition - Vector3(floorSize.x, -floorSize.x, 0.0f), wallSizeZ);
	AddWallToWorld(floorPosition + Vector3(0.0f, floorSize.z, floorSize.z), wallSizeX);
	AddWallToWorld(floorPosition - Vector3(0.0f, -floorSize.z, floorSize.z), wallSizeX);
	//Islands
	AddFloorToWorld(floorPosition + floorSize * 0.85f, islandSize, islandColour, CollisionResolution::Impulse);
	AddFloorToWorld(floorPosition - Vector3(floorSize.x, -floorSize.y, floorSize.z) * 0.85f, islandSize, islandColour, CollisionResolution::Impulse);
	//Terrain
	AddFloorToWorld(Vector3(0, -2, 0), Vector3(500, 3, 300), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(400, 5, -200), Vector3(100, 5, 100), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(420, 15, -220), Vector3(80, 5, 80), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(440, 25, -240), Vector3(60, 5, 60), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(460, 35, -260), Vector3(40, 5, 40), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-400, 5, 200), Vector3(100, 5, 100), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-420, 15, 220), Vector3(80, 5, 80), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-440, 25, 240), Vector3(60, 5, 60), terrainColour, CollisionResolution::Impulse);
	AddFloorToWorld(Vector3(-460, 35, 260), Vector3(40, 5, 40), terrainColour, CollisionResolution::Impulse);
	//Trampoline
	AddTrampolineToWorld(Vector3(0, 5, 0));
	//Goose
	playerCharacter = AddGooseToWorld(Vector3(425, 5, 425));
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

void NCL::CSC8503::CourseworkGame::AddTrampolineToWorld(const Vector3& position)
{
	AddFloorToWorld(position, Vector3(50, 3, 50), Vector4(1, 0, 0, 1), CollisionResolution::Spring);
	AddWallToWorld(position + Vector3(50, 0, 3), Vector3(3, 5, 50));
	AddWallToWorld(position - Vector3(50, 0, 3), Vector3(3, 5, 50));
	AddWallToWorld(position + Vector3(-3, 0, 50), Vector3(50, 5, 3));
	AddWallToWorld(position - Vector3(-3, 0, 50), Vector3(50, 5, 3));
}

GameObject* CourseworkGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);
	sphere->GetTransform().SetWorldScale(sphereSize);
	sphere->GetTransform().SetWorldPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);

	if (rand() % 2)
	{
		sphere->GetPhysicsObject()->InitSphereInertia();
		sphere->GetPhysicsObject()->SetElasticity(0.1f);
		sphere->GetPhysicsObject()->SetBuoyancy(100.0f);
		sphere->GetPhysicsObject()->SetGravityAffinity(false);
		//Just to see which ones are full
		Vector4 colour = Vector4(1.0f, 1.0f, 0.0f, 1.0f);
		sphere->GetRenderObject()->SetColour(colour);
		sphere->GetRenderObject()->SetOriginalColour(colour);
	}
	else
	{
		sphere->GetPhysicsObject()->InitHollowSphereInertia();
		sphere->GetPhysicsObject()->SetElasticity(0.9f);
		sphere->GetPhysicsObject()->SetBuoyancy(1000.0f);
		//Just to see which ones are hollow
		Vector4 colour = Vector4(1.0f, 0.0f, 1.0f, 1.0f);
		sphere->GetRenderObject()->SetColour(colour);
		sphere->GetRenderObject()->SetOriginalColour(colour);
	}

	world->AddGameObject(sphere);

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
	cube->GetPhysicsObject()->SetElasticity(0.5f);
	cube->GetPhysicsObject()->SetBuoyancy(50.0f);

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(cube);

	return cube;
}

GameObject* CourseworkGame::AddGooseToWorld(const Vector3& position)
{
	float size			= 1.0f;
	float inverseMass	= 0.25f;

	GameObject* goose = new GameObject();


	SphereVolume* volume = new SphereVolume(size);
	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform().SetWorldScale(Vector3(size,size,size) );
	goose->GetTransform().SetWorldPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));

	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->GetPhysicsObject()->SetBuoyancy(130);
	goose->GetPhysicsObject()->SetElasticity(0.7f);
	goose->GetPhysicsObject()->SetCollisionResolution(CollisionResolution::Impulse | CollisionResolution::Spring);

	world->AddGameObject(goose);

	return goose;
}

GameObject* CourseworkGame::AddParkKeeperToWorld(const Vector3& position)
{
	float meshSize = 4.0f;
	float inverseMass = 0.5f;

	GameObject* keeper = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3, 0.9f, 0.3) * meshSize);
	keeper->SetBoundingVolume((CollisionVolume*)volume);

	keeper->GetTransform().SetWorldScale(Vector3(meshSize, meshSize, meshSize));
	keeper->GetTransform().SetWorldPosition(position);

	keeper->SetRenderObject(new RenderObject(&keeper->GetTransform(), keeperMesh, nullptr, basicShader));
	keeper->SetPhysicsObject(new PhysicsObject(&keeper->GetTransform(), keeper->GetBoundingVolume()));

	keeper->GetPhysicsObject()->SetInverseMass(inverseMass);
	keeper->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(keeper);

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

GameObject* CourseworkGame::AddAppleToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.7f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetWorldScale(Vector3(4, 4, 4));
	apple->GetTransform().SetWorldPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), appleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

