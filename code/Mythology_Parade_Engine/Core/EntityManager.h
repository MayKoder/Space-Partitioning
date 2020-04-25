#ifndef _ENTITYMANAGER_H
#define _ENTITYMANAGER_H

#define MAX_BUILDING_TYPES 8

#include "j1Module.h"
#include <unordered_map>
#include "Entity.h"
#include"j1Input.h"
#include"j1Map.h"
#include"j1Pathfinding.h"
#include"j1Audio.h"

#include<vector>
#include <algorithm>

//Can delete
#include "j1Scene.h"
#include"j1Textures.h"
#include "Player.h"



enum class UnitType;
enum BuildingType;
enum CivilizationType {
	VIKING,
	GREEK,
	NONE
};

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

	////Called when loading the game
	//bool Load(pugi::xml_node&);

	////Called when saving the game
	//bool Save(pugi::xml_node&) const;

	bool DeleteEntity(Entity*);

	Entity* CreatePlayerEntity();
	Entity* CreateUnitEntity(UnitType, iPoint, CivilizationType);
	Entity* CreateBuildingEntity(iPoint, BuildingType, BuildingInfo, CivilizationType);
	void UpdateBuildPreview(int);
	void SetBuildIndex(int);

	//Load data packets
	void LoadBuildingsData(pugi::xml_node&);
	iPoint CalculateBuildingSize(int, int, int);
	void DrawEverything();

	void LoadBuildingsBlitRect();

	Player* getPlayer() const;

	static bool IsPointInsideQuad(SDL_Rect rect, int x, int y);

public:

	std::unordered_map<EntityType, std::list<Entity*>> entities;
	CreationPreview crPreview;
	SDL_Texture* debugTex;

	//The way to store the spritesheets
	std::unordered_map<SpriteSheetType, SDL_Texture*> entitySpriteSheets;
	std::vector<BuildingInfo> buildingsData;
	void FxUnits(int channel, int fx, int posx, int posy);
	int volume;

private:
	int buildingTestIndex = 0;

public:
	SDL_Rect constructorSpriteRect;
	SDL_Rect destructedSpriteRect;
	SDL_Rect construction_bar_back;
	SDL_Rect construction_bar_empty;
	SDL_Rect construction_bar_front;
	SDL_Rect life_bar_front;
	SDL_Rect research_bar_front;

	int Building_destruction;
	int Building_placed;
	int Decrease_Faith;
	int Getting_resources;
	int hit_1;
	int Increase_faith;
	int increase_prayers;
	int increase_sacrifice;
	int Walking_troops;
	int CreateMonk_sound;
	int CreateAssasin_sound;
	int Research_sound;

};
#endif // !_ENTITYMANAGER_H
