#pragma once
#include "CourseworkGame.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public CourseworkGame, public PacketReceiver {
		public:
			NetworkedGame();
			virtual ~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			void SpawnPlayer(int id, const Vector3& position);

			void StartLevel();

			void GenerateNetworkObjects();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void MoveEnemies(float dt) override;

			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;

			std::vector<NetworkObject*> networkObjects;
			std::vector<GooselandObject*> islands;

			std::map<int, GooseObject*> serverPlayers;
			GooseObject* localPlayer;
		};
	}
}

