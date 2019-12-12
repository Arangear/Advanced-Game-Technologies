#pragma once
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

class GooseObject : public GameObject
{
public:
	GooseObject() : GameObject() {}
	virtual ~GooseObject() {}

	int GetItemCount() const;

	//Returns ID of the picked up object;
	int PickUpItem(GameObject* object);

	void UpdatePositions();

	void AddPoints(const unsigned int value)
	{
		points += value;
	}

	unsigned int GetPoints() const
	{
		return points;
	}

	vector<GameObject*>& GetItems()
	{
		return pickedUpItems;
	}
	//Do we possess stolen goods?
	bool CommitedTheft() const
	{
		return commitedTheft;
	}
	//We are upstanding citizens now
	void DropAllCharges()
	{
		commitedTheft = false;
	}

	void ManageAbilities(const float dt);

	int& GetSpeedMultiplier() { return speedMultiplier; }
	float& GetSprint() { return sprint; }
	float& GetSprintCD() { return sprintCD; }
	float& GetQuackAttackCD() { return quackAttackCD; }
	float& GetQuackForce() { return quackForce; }
	float& GetQuackRange() { return quackRange; }
	int& GetQuackID() { return quackID; }
	vector<std::pair<string, std::pair<int, int>>> GetMessages() const { return messages; }

protected:

	int speedMultiplier = 1;
	float sprint = 0.0f;
	float sprintCD = 0.0f;
	float quackAttackCD = 0.0f;
	float quackForce = 5000.0f;
	float quackRange = 70.0f;
	int quackID = 0;
	vector<std::pair<std::string, std::pair<int, int>>> messages = {	std::pair<string, std::pair<int, int>>("Quack!", std::pair<int, int>(560, 230)),
																		std::pair<string, std::pair<int, int>>("QUAAARRGH!", std::pair<int, int>(500, 230)),
																		std::pair<string, std::pair<int, int>>("Quack off!", std::pair<int, int>(500, 230)),
																		std::pair<string, std::pair<int, int>>("Geese lives matter!", std::pair<int, int>(380, 230)),
																		std::pair<string, std::pair<int, int>>("Brexit!", std::pair<int, int>(540, 230)),
																		std::pair<string, std::pair<int, int>>("Quack at me mate!", std::pair<int, int>(400, 230)) };

	vector<GameObject*> pickedUpItems;
	unsigned int points = 0;
	//Have we stolen not an apple?
	bool commitedTheft = false;
};