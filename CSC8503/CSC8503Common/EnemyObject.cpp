#include "EnemyObject.h"

void EnemyObject::UpdatePosition(GooseObject*& goose, float dt)
{
	nodes.clear();
	if (goose->CommitedTheft())
	{//Goose found guilty
		NavigationPath path;
		if ((*grid).FindPath(this->GetConstTransform().GetWorldPosition(), goose->GetConstTransform().GetWorldPosition(), path))
		{//Path to justice found
			Vector3 pos;
			while (path.PopWaypoint(pos))
			{
				nodes.push_back(pos);
			}
			if (nodes.size() >= 2)
			{
				Vector3 direction = nodes[1] - nodes[0];
				direction.Normalise();
				this->GetTransform().SetWorldPosition(this->GetConstTransform().GetWorldPosition() + direction * speed * dt);
				//this->GetTransform().SetLocalOrientation()
			}
		}
	}
}
