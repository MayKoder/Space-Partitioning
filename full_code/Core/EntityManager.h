#ifndef _ENTITYMANAGER_H
#define _ENTITYMANAGER_H

#define MAX_BUILDING_TYPES 8

#include "j1Module.h"
#include <unordered_map>
#include "Entity.h"
#include"j1Input.h"
#include"j1Map.h"

#include<vector>
#include <algorithm>

//Can delete
#include "j1Scene.h"
#include"j1Textures.h"
#include"MaykMath.h"

class Entity;

//Temporal sorting function
struct entity_Sort
{
	inline bool operator() (Entity* struct1, Entity* struct2)
	{
		return (struct1->position.y < struct2->position.y);
	}
};

class EntityManager : public j1Module
{
public:

	EntityManager();

	// Destructor
	~EntityManager();

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool DeleteEntity(Entity*);


	//Entity creation
	Entity* CreateUnitEntity(iPoint);
	Entity* CreateBuildingEntity(iPoint);

	//Load data packets
	iPoint CalculateBuildingSize(int, int, int);
	void DrawEverything();

	static bool IsPointInsideQuad(SDL_Rect rect, int x, int y);
	SDL_Rect SDLRectToMaykRect(Rect r);

public:

	std::unordered_map<EntityType, std::list<Entity*>> entities;

	Entity* selectedUnit;
	SDL_Texture* buildingTex;
	SDL_Texture* entTex;

	//TODO 1.1: Create tree variables
	QuadTree quadTree;
	AABBTree aabbTree;

};
#endif // !_ENTITYMANAGER_H
