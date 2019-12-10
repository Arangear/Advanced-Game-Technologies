#include "EnemyObject.h"

void EnemyObject::UpdatePosition(GooseObject*& goose, float dt)
{
	NavigationPath path;
	nodes.clear();
	if (goose->CommitedTheft())
	{//Goose found guilty
		targetPosition = goose->GetConstTransform().GetWorldPosition();
		targetPosition.y = 0;
		if ((*grid).FindPath(this->GetConstTransform().GetWorldPosition(), goose->GetConstTransform().GetWorldPosition(), path))
		{//Path to justice found
			Vector3 pos;
			while (path.PopWaypoint(pos))
			{
				nodes.push_back(pos);
			}

			optimiseNodes();
			drawPath();

			if (nodes.size() >= 2) //Sanity check
			{
				Vector3 direction = nodes[1] - nodes[0];
				direction.Normalise();
				this->GetPhysicsObject()->AddForce(direction * speed * dt);
			}
		}
	}
	else
	{//No criminals in sight
		targetPosition = origin;
		targetPosition.y = 0;
		(*grid).FindPath(this->GetConstTransform().GetWorldPosition(), origin, path);
		Vector3 pos;
		while (path.PopWaypoint(pos))
		{
			nodes.push_back(pos);
		}

		optimiseNodes();
		drawPath();

		if (nodes.size() >= 2) //Sanity check
		{
			Vector3 direction = nodes[1] - nodes[0];
			direction.Normalise();
			this->GetPhysicsObject()->AddForce(direction * speed * dt);
		}
	}
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
	RayCollision collision;
	vector<Vector3> optimisedNodes;
	optimisedNodes.push_back(nodes[0]);
	for (int i = 0; i < nodes.size() - 1;)
	{
		for (int j = i + 1; j < nodes.size(); j++)
		{
			Vector3 direction = nodes[j] - nodes[i];
			direction.Normalise();
			Ray ray(nodes[i], direction);
			//Find if there are obstacles on the path
			if (raycast(ray, collision, true))
			{
				float distance = (nodes[j] - nodes[i]).Length();
				if (collision.rayDistance < distance && i != j - 1) //Obstacle detected and sanity check
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
				ray = Ray::Ray(nodes[i], direction);
				if (raycast(ray, collision, true))
				{
					float distance = (targetPosition - nodes[i]).Length();
					if (collision.rayDistance >= distance) //No obstacles on the way directly to target
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
