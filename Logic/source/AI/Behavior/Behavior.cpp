#include <AI\Behavior\Behavior.h>
#include <AI\Enemy.h>
#include <Misc\RandomGenerator.h>

#define SCALAR_DIR 0.3f
#define SCALAR_ALIGN 1.1f
#define SCALAR_COHES 0.7f
#define SCALAR_SEP 2.3f

#define MAX_LEN_FOR_SEPERATION 10.f
// this can be changed in the future maybe who knows
#define CHANGE_NODE_DIST 0.3f

#include <queue>
using namespace Logic;

Behavior::Behavior(PathingType type)
{
	m_pathingType = type;
}

Behavior::~Behavior()
{
}

void Behavior::update(Enemy &enemy, std::vector<Enemy*> const &closeEnemies, Player const & player, float deltaTime)
{
	updateSpecific(enemy, closeEnemies, player, deltaTime);

	// this is frame bound, idk how to fix that tho
	RunIn in{ &enemy, closeEnemies, &player, this, deltaTime };
	runTree(in);
}

void Behavior::walkPath(RunIn &in)
{
	DirectX::SimpleMath::Vector3 pathNode = m_pathing.getNode();
	btVector3 node { pathNode.x, pathNode.y, pathNode.z };
	btVector3 dir = node - in.enemy->getPositionBT();
	
	boidCalculations(in.enemy->getPositionBT(), 
		dir, in.closeEnemies, in.enemy->getMoveSpeed(), in.deltaTime);

	in.enemy->getRigidBody()->translate(dir);

	if ((node - in.enemy->getPositionBT()).length() < CHANGE_NODE_DIST
		&& !m_pathing.pathOnLastNode())
	{
		m_pathing.setCurrentNode(m_pathing.getCurrentNode() + 1);
		m_changedGoalNode = true;
	}
}

void Behavior::boidCalculations(btVector3 &pos, btVector3 &dir,
	std::vector<Enemy*> const &close, float maxSpeed, float dt)
{
	btVector3 sep, align, cohes;
	int totalSep = 0;
	if (close.size() == 0)
		return; // just move
	// make the vectors
	btVector3 temp;
	for (auto const &enemy : close)
	{
		align += enemy->getRigidBody()->getLinearVelocity();

		cohes += enemy->getPositionBT();

		temp = pos - enemy->getPositionBT();
		if (temp.length() < MAX_LEN_FOR_SEPERATION)
		{
			sep += temp;
			totalSep++;
		}
	}
	
	// SEPERATION (Steer away from the group)
	cohes /= close.size();
	cohes = cohes - pos;
	cohes = cohes.normalize();

	// ALIGNMENT (Have same vel as group)
	align /= close.size();
	align = align.normalize();

	// COHESION (Stay towards group position)
	sep /= totalSep + 1;
	sep = sep.normalize();

	// RET
	dir = dir * SCALAR_DIR + cohes * SCALAR_COHES + align * SCALAR_ALIGN + sep * SCALAR_SEP;
	dir.setY(0); // right now y should not be changed
	dir.normalize();
	dir *= maxSpeed * (dt * 0.001f);
}

void Behavior::runTree(RunIn &in)
{
	runNode(in, m_root);
}

void Behavior::setRoot(NodeType type, int value, run func)
{
	m_root = { type, value, {}, func };
}

Behavior::BehaviorNode* Behavior::addNode(BehaviorNode *parent,
	NodeType type, int value, run func)
{
	int index = parent->children.size();
	parent->children.push_back({ type, value, {}, func });

	if (parent->type == PRIORITY)
	{
		while (index > 0 &&
			parent->children[index].value > parent->children[index - 1].value)
		{
			std::swap(parent->children[index], parent->children[index - 1]);
			index--; // weird..
		}
	}

	return &parent->children[index];
}

bool Behavior::runNode(RunIn &in, BehaviorNode &node)
{
	// Recursive method to run the entire tree, todo: opt
	int totalWeight = 0, curr = 0, rng;
	switch (node.type)
	{
		case NodeType::ACTION: // Action, just run the node
			return node.run(in);
			break;
		case NodeType::RANDOM: // Random selector, run a random node
			for (BehaviorNode &n : node.children)
				totalWeight += n.value;
			rng = RandomGenerator::singleton().getRandomInt(0, totalWeight);
			for (BehaviorNode &n : node.children)
			{
				curr += n.value;
				if (rng <= curr)
					return runNode(in, n);
			}
			break;
		case NodeType::CONDITION: // Run the node, if true run children in seq
			if (node.run(in))
				for (BehaviorNode &n : node.children)
					runNode(in, n);
			else
				return false;
			break;
		case NodeType::PRIORITY: // run the most priority node, if it failed run next most value etc etc
			// this is already sorted
			for (BehaviorNode &n : node.children)
				if (runNode(in, n))
					return true;
			break;
		case NodeType::SEQUENCE: // run all the nodes
			for (BehaviorNode &n : node.children)
				runNode(in, n);
			break;
	}
	return true;
}

bool Behavior::isGoalChangedAndSetToFalse()
{
	bool nodeChange = m_changedGoalNode;
	m_changedGoalNode = false;
	return nodeChange;
}

Behavior::BehaviorNode* Behavior::getRoot() {
	return &m_root; 
}

Pathing& Behavior::getPath()
{
	return m_pathing;
}

Behavior::PathingType Behavior::getPathingType() const
{
	return m_pathingType;
}

void Behavior::setPathingType(PathingType pathingType)
{
	m_pathingType = pathingType;
}
