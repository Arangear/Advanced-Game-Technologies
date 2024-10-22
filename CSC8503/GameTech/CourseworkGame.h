#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"
#include "../../CSC8503/CSC8503Common/EnemyObject.h"
#include "../../CSC8503/CSC8503Common/GooselandObject.h"


namespace NCL
{
	namespace CSC8503
	{
		class CourseworkGame
		{
		public:
			CourseworkGame();
			virtual ~CourseworkGame();

			virtual void UpdateGame(float dt);
			bool IsOver() { return gameFinished; }

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys(float dt);

			void InitWorld();

			void QuackAttack();

			void EndGame(float dt);
			void DisplayGrid();

			void MovePlayerCharacter(float dt);
			virtual void MoveEnemies(float dt);
			void CameraMovement();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& scale, const Vector4& colour, const int collisionResolution);
			GameObject* AddIslandToWorld(const Vector3& position, const Vector3& scale, const Vector4& colour, const int collisionResolution);
			GameObject* AddWallToWorld(const Vector3& position, const Vector3& scale);
			GameObject* AddRampToWorld(const Vector3& position, const Vector3& scale, const Vector3& rotation, const float inverseMass);
			void AddGateToWorld(const Vector3& position, const Vector3& rotation);
			void AddTrampolineToWorld(const Vector3& position);

			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			//IT'S HAPPENING
			GooseObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);

			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			float		forceMagnitude;

			std::set<PickableObject*> pickables;
			std::vector<EnemyObject*> enemies;
			std::vector<GameObject*> obstacles;
			std::vector<GameObject*> movables;

			float timer = 180.0f;
			bool gameFinished = false;
			bool toggleGrid = false;

			void DrawDisplay(float dt);

			OGLMesh*	cubeMesh	= nullptr;
			OGLMesh*	sphereMesh	= nullptr;
			OGLTexture* basicTex	= nullptr;
			OGLShader*	basicShader = nullptr;

			//Coursework Meshes
			OGLMesh*	gooseMesh	= nullptr;
			OGLMesh*	keeperMesh	= nullptr;
			OGLMesh*	appleMesh	= nullptr;
			OGLMesh*	charA		= nullptr;
			OGLMesh*	charB		= nullptr;

			//Camera and gameplay stuff
			GooseObject* playerCharacter	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 5, -20);
		};
	}
}

