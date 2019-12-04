#pragma once
#include "GameTechRenderer.h"
#include "../CSC8503Common/PhysicsSystem.h"


namespace NCL
{
	namespace CSC8503
	{
		class CourseworkGame
		{
		public:
			CourseworkGame();
			~CourseworkGame();

			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys(float dt);

			void InitWorld();

			void MovePlayerCharacter(float dt);
			void CameraMovement();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& scale, const Vector4& colour, const int collisionResolution);
			GameObject* AddWallToWorld(const Vector3& position, const Vector3& scale);
			void AddTrampolineToWorld(const Vector3& position);

			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f);
			//IT'S HAPPENING
			GameObject* AddGooseToWorld(const Vector3& position);
			GameObject* AddParkKeeperToWorld(const Vector3& position);
			GameObject* AddCharacterToWorld(const Vector3& position);
			GameObject* AddAppleToWorld(const Vector3& position);


			GameTechRenderer*	renderer;
			PhysicsSystem*		physics;
			GameWorld*			world;

			float		forceMagnitude;
			float sprint = 0.0f;
			float sprintCD = 0.0f;

			void ManageSprint(float dt);

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
			GameObject* playerCharacter	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 5, -20);
		};
	}
}

