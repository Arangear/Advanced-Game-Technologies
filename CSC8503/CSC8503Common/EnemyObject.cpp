#include "EnemyObject.h"

EnemyObject::EnemyObject(string filePath) : GameObject()
{
	grid = new NavigationGrid(filePath);
	stateMachine = new StateMachine();
	
	StateFunc chaseFunc = [](void* data)
	{
		EnemyObject* object = (EnemyObject*)data;
		NavigationPath path;
		object->nodes.clear();
		object->targetPosition = object->chasedGoose->GetConstTransform().GetWorldPosition();
		object->targetPosition.y = 0;
		if (object->grid->FindPath(object->GetConstTransform().GetWorldPosition(), object->chasedGoose->GetConstTransform().GetWorldPosition(), path))
		{//Path to justice found
			Vector3 pos;
			while (path.PopWaypoint(pos))
			{
				object->nodes.push_back(pos);
			}

			object->optimiseNodes();
			object->drawPath();

			if (object->nodes.size() >= 2) //Sanity check
			{
				Vector3 direction = object->nodes[1] - object->nodes[0];
				direction.Normalise();
				object->GetPhysicsObject()->AddForce(direction * object->speed * object->currentDT);
			}
		}
	};
	StateFunc returnFunc = [](void* data)
	{
		EnemyObject* object = (EnemyObject*)data;
		NavigationPath path;
		object->nodes.clear();
		object->targetPosition = object->origin;
		object->targetPosition.y = 0;
		object->grid->FindPath(object->GetConstTransform().GetWorldPosition(), object->origin, path);
		Vector3 pos;
		while (path.PopWaypoint(pos))
		{
			object->nodes.push_back(pos);
		}

		object->optimiseNodes();
		object->drawPath();

		if (object->nodes.size() >= 2) //Sanity check
		{
			Vector3 direction = object->nodes[1] - object->nodes[0];
			direction.Normalise();
			object->GetPhysicsObject()->AddForce(direction * object->speed * object->currentDT);
		}
	};

	GenericState* stateChase = new GenericState(chaseFunc, (void*)this);
	GenericState* stateReturn = new GenericState(returnFunc, (void*)this);

	stateMachine->AddState(stateReturn);
	stateMachine->AddState(stateChase);

	GenericTransition<bool&, bool>* chaseFromReturn = new GenericTransition<bool&, bool>(
		GenericTransition<bool&, bool>::EqualsTransition, stateSwitch, true, stateReturn, stateChase);

	GenericTransition<bool&, bool>* returnFromChase = new GenericTransition<bool&, bool>(
		GenericTransition<bool&, bool>::EqualsTransition, stateSwitch, false, stateChase, stateReturn);

	stateMachine->AddTransition(chaseFromReturn);
	stateMachine->AddTransition(returnFromChase);
}

void EnemyObject::UpdatePosition(GooseObject*& goose, float dt)
{
	chasedGoose = goose;
	currentDT = dt;
	stateSwitch = goose->CommitedTheft();

	stateMachine->Update();
}

void EnemyObject::OnCollisionBegin(GameObject* otherObject)
{
	if (dynamic_cast<GooseObject*>(otherObject))
	{
		for (GameObject* object : ((GooseObject*)otherObject)->GetItems())
		{
			((PickableObject*)object)->SetOwner(nullptr);
			((PickableObject*)object)->SetID(-1);
			((PickableObject*)object)->ResetPosition();
		}
		((GooseObject*)otherObject)->GetItems().clear();
		((GooseObject*)otherObject)->DropAllCharges();
	}
}

void EnemyObject::optimiseNodes()
{
	RayCollision collision[3];
	vector<Vector3> optimisedNodes;
	Vector3 currentPosition = this->GetConstTransform().GetWorldPosition();
	currentPosition.y = 0;
	optimisedNodes.push_back(currentPosition);
	for (int i = 0; i < nodes.size() - 1;)
	{
		for (int j = i + 1; j < nodes.size(); j++)
		{
			Vector3 direction = nodes[j] - nodes[i];
			direction.Normalise();

			//Approximate rays to prevent getting stuck in an object
			Ray ray1(nodes[i], direction);
			Ray ray2(nodes[i] + Vector3::Cross(direction, Vector3(0, 1, 0)), direction);
			Ray ray3(nodes[i] - Vector3::Cross(direction, Vector3(0, 1, 0)), direction);
			
			//Find if there are obstacles on the path
			if (raycast(ray3, collision[2], true) &&
				raycast(ray2, collision[1], true) &&
				raycast(ray1, collision[0], true))
			{
				float distance = (nodes[j] - nodes[i]).Length();
				if ((collision[2].rayDistance < distance ||
					collision[1].rayDistance < distance ||
					collision[0].rayDistance < distance) && i != j - 1) //Obstacle detected and sanity check
				{
					//Add previous node to the optimised path
					optimisedNodes.push_back(nodes[j - 1]);
					i = j - 1;
					break;
				}
			}
			//Reached the end of path without encountering obstacles
			if (j == nodes.size() - 1)
			{
				direction = targetPosition - nodes[i];
				direction.Normalise();
				ray1 = Ray::Ray(nodes[i], direction);
				if (raycast(ray1, collision[0], true))
				{
					float distance = (targetPosition - nodes[i]).Length();
					if (collision[0].rayDistance >= distance) //No obstacles on the way directly to target
					{
						optimisedNodes.push_back(targetPosition);
						nodes = optimisedNodes;
						return;
					}
				}
				//There was an obstacle so we need to go around
				optimisedNodes.push_back(nodes[j]);
				optimisedNodes.push_back(targetPosition);
				nodes = optimisedNodes;
				return;
			}
		}
	}
}

bool EnemyObject::raycast(Ray& r, RayCollision& closestCollision, bool object) const
{
	RayCollision collision;

	for (auto& i : obstacles)
	{
		if (!i->GetBoundingVolume())
		{ //objects might not be collideable etc...
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision))
		{

			if (!object)
			{
				closestCollision = collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision = collision;
		closestCollision.node = collision.node;
		return true;
	}
	return false;
}

void EnemyObject::drawPath()
{
	for (int i = 0; i < nodes.size() - 1; i++)
	{
		Debug::DrawLine(nodes[i] + Vector3(0, 1, 0), nodes[i + 1] + Vector3(0, 1, 0), Vector4(1, 0, 0, 1));
	}
}
