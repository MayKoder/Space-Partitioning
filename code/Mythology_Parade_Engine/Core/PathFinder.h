#ifndef __PATHFINDER_H__
#define __PATHFINDER_H__
#include "p2Point.h"
#include <vector>
#include"Entity.h"

#pragma region Structs

struct PathRequest 
{
	iPoint origin;
	iPoint destination;
	std::list <Entity*> requestEntity;

	PathRequest(iPoint, iPoint, std::list <Entity*>);
};

struct PathList;
struct PathNode
{
	// Convenient constructors
	PathNode();
	PathNode(int g, int h, const iPoint& pos, const PathNode* parent);
	PathNode(const PathNode& node);

	// Fills a list (PathList) of all valid adjacent pathnodes
	uint FindWalkableAdjacents(PathList& list_to_fill) const;
	// Calculates this tile score
	int Score() const;
	// Calculate the F for a specific destination tile
	int CalculateF(const iPoint& destination);

	// -----------
	int g;
	int h;
	iPoint pos;
	const PathNode* parent; // needed to reconstruct the path in the end
};

// ---------------------------------------------------------------------
// Helper struct to include a list of path nodes
// ---------------------------------------------------------------------
struct PathList
{

	// Looks for a node in this list and returns it's list node or NULL
	std::vector<PathNode>::const_iterator* Find(const iPoint& point) const;

	// Returns the Pathnode with lowest score in this list or NULL if empty
	PathNode* GetNodeLowestScore();

	// -----------
	// The list itself, note they are not pointers!
	std::vector<PathNode> list;

};
#pragma endregion

// TODO 1: Just take a few minutes to undertand this new class and think from where it comes

class PathFinder
{
private:
	bool IteratePath();

public:
	PathFinder();
	~PathFinder();

	// Main function to request a path from A to B
	void PreparePath(const iPoint& origin, const iPoint& destination, std::list <Entity*> req);


	// To request all tiles involved in the last generated path
	const std::vector<iPoint>* GetLastPath() const;

	std::vector<iPoint> last_path;

	bool Update();

	bool pathCompleted;
	bool available;

private:

	PathList open;
	PathList close;

	iPoint origin;
	iPoint destination;

	std::list<Entity*> requestUnitsList;

	int max_iterations;

	// we store the created path here
};

#endif // !__PATHFINDER_H__

