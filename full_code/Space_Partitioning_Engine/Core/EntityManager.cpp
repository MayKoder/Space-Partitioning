#include "EntityManager.h"
#include "Building.h"
#include "j1Gui.h"
#include "Unit.h"

#include "p2Log.h"
EntityManager::EntityManager()
{
	name.append("entity_manager");
	buildingsData.reserve(MAX_BUILDING_TYPES);
	buildingTestIndex = 0;
}

//Destructor
EntityManager::~EntityManager()
{}

//Called before render is available
bool EntityManager::Awake(pugi::xml_node& a)
{

	//Load buildings info
	pugi::xml_document buildings;
	buildings.load_file(a.child("buildings").attribute("file").as_string());
	LoadBuildingsData(buildings.child("map").child("objectgroup"));

	//INFO: This is a good way to itinerate all the map, to itinerate only items in one key, use only the second for loop
	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			Entity* ent = it._Ptr->_Myval;
			ent->Awake(a.child(ent->name.c_str()));
		}
	}
	active = false;

	return true;
}

// Called before the first frame
bool EntityManager::Start()
{
	//TODO: NO HARDCODE BOY
	entitySpriteSheets[SpriteSheetType::BUILDINGS] = App->tex->Load("assets/buildings/Buildings.png");

	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			it._Ptr->_Myval->Start();
		}
	}

	return true;
}

void EntityManager::LoadBuildingsBlitRect()
{
	for (unsigned int i = 0; i < buildingsData.size(); i++)
	{
		BuildingInfo* info = &buildingsData[i];
		int blitWidth = info->tileLenght * App->map->data.tile_width;
		info->blitSize = CalculateBuildingSize(blitWidth, info->spriteRect.w, info->spriteRect.h);
	}
}

// Called each loop iteration
bool EntityManager::PreUpdate()
{
	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			it._Ptr->_Myval->PreUpdate();
		}
	}
	return true;
}

// Called each loop iteration
bool EntityManager::Update(float dt)
{
	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			it._Ptr->_Myval->Update(dt);
		}
	}

	//TODO: Move this logic to the player
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		EnterBuildMode();
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		if (buildingTestIndex < MAX_BUILDING_TYPES - 1)
		{
			buildingTestIndex++;
		}
		else
		{
			buildingTestIndex = 0;
		}
		UpdateBuildPreview(buildingTestIndex);
	}

	if (crPreview.active)
	{
		iPoint mouse = App->map->GetMousePositionOnMap();

		crPreview.canBuild = true;
		debugTex = App->scene->debugBlue_tex;

		for (int i = 0; i <= 1; i++)
		{
			for (int y = mouse.y; y > mouse.y - crPreview.height; y--)
			{
				for (int x = mouse.x; x < mouse.x + crPreview.width; x++)
				{
					if (i == 0)
					{
						if (crPreview.canBuild)
						{
							debugTex = App->scene->debugRed_tex;
							crPreview.canBuild = false;
						}
					}
					else
					{
						if (IN_RANGE(x, 0, App->map->data.width - 1) == 1 && IN_RANGE(y, 0, App->map->data.height - 1) == 1)
						{
							iPoint p = App->map->MapToWorld(x, y);
							App->render->Blit(debugTex, p.x, p.y);
						}
					}
				}
			}
		}
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && crPreview.active && crPreview.canBuild)
	{
		int faithToDescrease = 0;
		iPoint mouse = App->map->GetMousePositionOnMap();
		iPoint spawnPos = App->map->MapToWorld(mouse.x, mouse.y);
		spawnPos.y += App->map->data.tile_height / 2;
		bool viking = false;
		switch (buildingTestIndex)
		{
		case 0:
			viking = true;
		case 4:
			CreateBuildingEntity(spawnPos, BuildingType::FORTRESS, buildingsData[buildingTestIndex],CivilizationType::VIKING);
			break;
		case 1:
			viking = true;
		case 5:
			CreateBuildingEntity(spawnPos, BuildingType::MONASTERY , buildingsData[buildingTestIndex],CivilizationType::VIKING);
			faithToDescrease = 200;
			break;
		case 2:
			viking = true;
		case 6:
			CreateBuildingEntity(spawnPos, BuildingType::TEMPLE, buildingsData[buildingTestIndex], CivilizationType::VIKING);
			break;
		case 3:
			viking = true;
		case 7:
			CreateBuildingEntity(spawnPos, BuildingType::ENCAMPMENT, buildingsData[buildingTestIndex], CivilizationType::VIKING);
			faithToDescrease = 200;
			break;
		}

		//Onces you build disable building mode

		crPreview.active = false;
	}

	return true;
}



bool EntityManager::PostUpdate()
{
	return true;
}

// Called before quitting
bool EntityManager::CleanUp()
{
	//Double for?
	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			Entity* ent = it._Ptr->_Myval;
			delete ent;
		}
		entities[(EntityType)i].clear();
	}
	for (unsigned int i = 0; i < entitySpriteSheets.size(); i++)
	{
		if(entitySpriteSheets[(SpriteSheetType)i])
			App->tex->UnLoad(entitySpriteSheets[(SpriteSheetType)i]);
	}
	entities.clear();

	return true;
}

Entity* EntityManager::CreateUnitEntity(UnitType type, iPoint pos, CivilizationType civilization)
{
	Entity* ret = nullptr;

	switch (type)
	{
	case UnitType::MONK:
		ret = new Unit(UnitType::MONK, pos);
		break;
	}
	ret->type = EntityType::UNIT;

	entities[EntityType::UNIT].push_back(ret);

	//DELETE: THIS
	entities[EntityType::UNIT].sort(entity_Sort());

	return ret;
}

void EntityManager::DrawEverything()
{
	float dt = App->GetDT();
	Entity* ent = nullptr;

	for (unsigned i = 1; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			ent = it._Ptr->_Myval;
			ent->Draw(dt);
		}
	}
}

Entity* EntityManager::CreateBuildingEntity(iPoint pos, BuildingType type, BuildingInfo info, CivilizationType civilization)
{
	Entity* ret = nullptr;
	switch (type)
	{
	case FORTRESS:
		ret = new Building(BuildingType::FORTRESS, pos, info);
		break;
	case MONASTERY:
		ret = new Building(BuildingType::MONASTERY, pos, info);
		break;
	case TEMPLE:
		ret = new Building(BuildingType::TEMPLE, pos, info);
		break;
	case ENCAMPMENT:
		ret = new Building(BuildingType::ENCAMPMENT, pos, info);
		break;
	}


	iPoint iso = pos;
	iso += App->map->GetTilesHalfSize();
	iso = App->map->WorldToMap(iso.x, iso.y);

	ret->type = EntityType::BUILDING;

	//TODO load spritesheet when needed only? first call of constructor of entity?
	entities[EntityType::BUILDING].push_back(ret);
	//TODO: sort elements only inside the screen (QuadTree)
	entities[EntityType::BUILDING].sort(entity_Sort());
	//std::sort(entities[EntityType::BUILDING].begin(), entities[EntityType::BUILDING].end(), entity_Sort());

	return ret;
}

void EntityManager::EnterBuildMode()
{
	crPreview.active = !crPreview.active;
	UpdateBuildPreview(buildingTestIndex);
}
void EntityManager::SetBuildIndex(int index)
{
	if (index < MAX_BUILDING_TYPES - 1) {
		buildingTestIndex = index;
	}

	UpdateBuildPreview(buildingTestIndex);
}

//Called when deleting a new Entity
bool EntityManager::DeleteEntity(Entity* e)
{
	if (e != nullptr)
	{
		entities[e->type].remove(e);
		delete e;
		return true;
	}
	return false;
}

void EntityManager::UpdateBuildPreview(int index)
{
	BuildingInfo data = buildingsData[index];
	crPreview.height = data.tileLenght;
	crPreview.width = data.tileLenght;
}

void EntityManager::LoadBuildingsData(pugi::xml_node& node)
{
	if (node != NULL)
	{
		pugi::xml_node obj;

		for (obj = node.child("object"); obj; obj = obj.next_sibling("object"))
		{

			BuildingInfo info;
			bool push = true;
			info.spriteRect = { obj.attribute("x").as_int(), obj.attribute("y").as_int(), obj.attribute("width").as_int(), obj.attribute("height").as_int() };

			pugi::xml_node data = obj.child("properties");
			if (data != NULL)
			{
				pugi::xml_node prop;
				for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
				{
					//OPT: Not the best way but meh

					std::string name = prop.attribute("name").as_string();
					if (name == "civilization")
					{
						CivilizationType type = (CivilizationType)prop.attribute("value").as_int();
						info.civilization = type;
						if (IN_RANGE(type, VIKING, GREEK) == 0)
						{
							push = false;
						}
					}
					else if (push == false && name == "consType")
					{
						break;
					}
					else if (name == "tileSquareLenght")
					{
						info.tileLenght = prop.attribute("value").as_int();
					}
					else if (name == "type")
					{
						info.buildingType = (BuildingType)prop.attribute("value").as_int();
					}
				}
			}
			//TODO: Find a wat to mesure this with the tileLenght
			info.blitSize = { info.spriteRect.w, info.spriteRect.h };

			if (push)
				buildingsData.push_back(info);
		}
	}


}

iPoint EntityManager::CalculateBuildingSize(int bw, int w, int h)
{
	return {bw , (bw * h) / w};
}

bool EntityManager::IsPointInsideQuad(SDL_Rect rect, int x, int y)
{

	if (x >= rect.x && x <= rect.x + rect.w && y >= rect.y + rect.h && y <= rect.y)
		return true;

	return false;
}