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



enum class UnitType;
enum BuildingType;
struct CreationPreview
{
	bool active = false;
	int width;
	int height;
	bool canBuild = false;

};

struct BuildingInfo
{
	CivilizationType civilization;
	SDL_Rect spriteRect;
	iPoint blitSize;

	BuildingType buildingType;
	int tileLenght;
};

enum class SpriteSheetType
{
	BUILDINGS,
	ASSASSIN,
	PRIEST,
	SPEAR_SOLDIER
};

class Entity;
class Player;

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

	void EnterBuildMode();

	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	bool DeleteEntity(Entity*);

	Entity* CreateUnitEntity(UnitType, iPoint, CivilizationType);
	Entity* CreateBuildingEntity(iPoint);
	void SetBuildIndex(int);

	//Load data packets
	iPoint CalculateBuildingSize(int, int, int);
	void DrawEverything();

	static bool IsPointInsideQuad(SDL_Rect rect, int x, int y);

public:

	std::unordered_map<EntityType, std::list<Entity*>> entities;
	CreationPreview crPreview;
	SDL_Texture* debugTex;


private:
	int buildingTestIndex = 0;


};
#endif // !_ENTITYMANAGER_H
