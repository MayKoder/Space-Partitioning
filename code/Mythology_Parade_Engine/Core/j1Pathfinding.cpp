#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"


j1PathFinding::j1PathFinding() : j1Module(), map(NULL), width(0), height(0), requestPath(false)
{
	name.append("pathfinding");


}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	for (int i = 0; i < pathfinderList.size(); i++)
	{
		pathfinderList[i].last_path.clear();
	}
	pathfinderList.clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}
void j1PathFinding::ChangeMapValue(const iPoint pos, int value) const
{
	if (IN_RANGE(value, 0, 1) == 1)
	{
		map[(pos.y * width) + pos.x] = value;
	}
}
// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
		pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if (CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}


// TODO 3: Remember, now we want to iterate from all PathFinders and check if it's available.

void j1PathFinding::RequestPath(const iPoint& origin, const iPoint& destination, std::list<Entity*> requestUnit)
{
	LOG("Requesting a path...");
	if (!IsWalkable(origin) || !IsWalkable(destination))
	{
		LOG("Invalid Path");
		return;
	}

	pathRequestList.push(PathRequest(origin, destination, requestUnit));
	LOG("Path added to Path Request List");
}

bool j1PathFinding::Start()
{
	//TODO 3: Add PathFinder to the vector.
	pathfinderList.push_back(PathFinder());
	pathfinderList.push_back(PathFinder());
	return true;
}



bool j1PathFinding::Update(float dt)
{
	//This part distribute PathRequests to PathFinders
	//Check if there are any path request
	while (!pathRequestList.empty())
	{
		//Check if there are any pathFinder available to take that request
		for (int i = 0; i < pathfinderList.size(); i++)
		{
			if (pathfinderList[i].available)
			{
				//Add pathRequest to pathFinder and delete it from the pathRequest list
				PathRequest request = pathRequestList.front();
				pathRequestList.pop();

				pathfinderList[i].PreparePath(request.origin, request.destination, request.requestEntity);
				break;
			}
		}
	}

	//Update PathFinders
	for (int i = 0; i < pathfinderList.size(); i++)
	{
		if (!pathfinderList[i].available)
			pathfinderList[i].Update();
	}
	return true;
}


// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------

