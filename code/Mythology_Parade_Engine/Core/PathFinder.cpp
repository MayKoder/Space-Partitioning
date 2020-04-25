#include "PathFinder.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Pathfinding.h"
#include"Unit.h"

PathFinder::PathFinder() : last_path(DEFAULT_PATH_LENGTH), pathCompleted(false), max_iterations(5), available(true)
{
	LOG("PathFinder created");
	last_path.clear();
}

PathFinder::~PathFinder()
{
}



void PathFinder::PreparePath(const iPoint& o, const iPoint& d, std::list <Entity*> req)
{
	// Add the origin tile to open
	if (open.GetNodeLowestScore() == NULL)
		open.list.push_back(PathNode(0, o.DistanceTo(destination), o, nullptr));

	uint iterations = 0;

	origin = o;
	destination = d;
	requestUnitsList = req;

	int currentDistance = destination.DistanceManhattan(origin);

	max_iterations = App->pathfinding->maxPathLenght / currentDistance;

	available = false;

}

bool PathFinder::IteratePath()
{
	//TODO 2: This function won't need a loop inside anymore, we are controlling this loop outside
	bool ret = true;

	PathNode* node = new PathNode(*open.GetNodeLowestScore());
	close.list.push_back(*node);
	open.list.erase(*open.Find(node->pos));


	if (node->pos == destination) {
		const PathNode* iterator = node;

		last_path.clear();
		// Backtrack to create the final path
		for (iterator; iterator->pos != origin; iterator = iterator->parent)
		{
			last_path.push_back(iterator->pos);
		}

		last_path.push_back(origin);

		// Use the Pathnode::parent and Flip() the path when you are finish
		std::reverse(last_path.begin(), last_path.end());
		pathCompleted = true;
		available = true;
		open.list.clear();
		close.list.clear();

		bool walkingToEnemy = false;
		for (std::list<Entity*>::iterator it = requestUnitsList.begin(); it != requestUnitsList.end(); ++it)
		{
			Unit* unit = (Unit*)it._Ptr->_Myval;

			if (!walkingToEnemy && unit->enemyTarget != nullptr) 
			{
				last_path.pop_back();
				walkingToEnemy = true;
			}

			unit->SetPath(last_path);
		}
		requestUnitsList.clear();

		LOG("Path finished");
		//requestUnit->SetPath(last_path);

		RELEASE(node);
		return false;
	}


	PathList adjacentNodes;
	uint numNodes = node->FindWalkableAdjacents(adjacentNodes);


	for (uint i = 0; i < numNodes; i++)
	{
		// ignore nodes in the closed list
		if (close.Find(adjacentNodes.list[i].pos) == NULL) {
			// If it is NOT found, calculate its F and add it to the open list
			if (open.Find(adjacentNodes.list[i].pos) == NULL) {
				adjacentNodes.list[i].CalculateF(destination);
				open.list.push_back(adjacentNodes.list[i]);
			}
			// If it is already in the open list, check if it is a better path (compare G)
			else {
				if (adjacentNodes.list[i].g < open.Find(adjacentNodes.list[i].pos)->_Ptr->g) {
					// If it is a better path, Update the parent
					adjacentNodes.list[i].CalculateF(destination);
					open.list.erase(*open.Find(adjacentNodes.list[i].pos));
					open.list.push_back(adjacentNodes.list[i]);
				}
			}
		}
	}
	return ret;
}



// To request all tiles involved in the last generated path
const std::vector<iPoint>* PathFinder::GetLastPath() const
{
	return &last_path;
}

bool PathFinder::Update()
{
	//TODO 2: Make a loop to take control on how many times the function "IteratePath" should be called in one frame
	bool ret = true;

	j1Timer timer;

	uint32 startTime = timer.Read();
	for (int i = 0; i < max_iterations && ret; i++)
	{
		ret = IteratePath();

		//If the path is calculated or it's taking more than 2ms to calculate, stop
		//This 2ms limit if for all the paths, if 2 paths take 2ms, the other paths will have to wait for the next frame
		if (ret == false || timer.Read() - startTime >= 2)
			break;
	}
	return ret;
}


#pragma region PathList



// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
std::vector<PathNode>::const_iterator* PathList::Find(const iPoint& point) const
{
	for (std::vector<PathNode>::const_iterator it = list.begin(); it != list.end(); ++it) {
		if (it->pos == point)
			return &it;
	}
	return NULL;

}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
PathNode* PathList::GetNodeLowestScore()
{
	PathNode* ret = NULL;
	int min = 65535;

	if (list.empty()) {
		return ret;
	}


	for (int i = list.size() - 1; i >= 0; i--)
	{
		if (list[i].Score() < min) {
			min = list[i].Score();
			ret = &list[i];
		}
	}

	//while (item)
	//{
	//	if (item->Score() < min)
	//	{
	//		min = item->Score();
	//		ret = item;
	//	}
	//	item = item--;
	//}
	return ret;
}
#pragma endregion

#pragma region PathNode



// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.size();

	// north
	cell.create(pos.x, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// north - east
	cell.create(pos.x + 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(0, -1, cell, this));
	//	
	//// north - west
	cell.create(pos.x - 1, pos.y + 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(0, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// south - east
	cell.create(pos.x + 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	//// south -west
	cell.create(pos.x - 1, pos.y - 1);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if (App->pathfinding->IsWalkable(cell))
		list_to_fill.list.push_back(PathNode(-1, -1, cell, this));

	return list_to_fill.list.size();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceManhattan(destination);

	return g + h;
}
#pragma endregion

PathRequest::PathRequest(iPoint o, iPoint d, std::list<Entity*> req)
{
	origin = o;
	destination = d;
	requestEntity = req;
}
