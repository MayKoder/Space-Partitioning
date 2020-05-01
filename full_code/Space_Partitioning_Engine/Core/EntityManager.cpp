#include "EntityManager.h"
#include "Building.h"
#include "j1Gui.h"
#include "Unit.h"

#include "p2Log.h"
EntityManager::EntityManager() : selectedUnit(nullptr), buildingTex(nullptr), entTex(nullptr)
{
	name.append("entity_manager");
}

//Destructor
EntityManager::~EntityManager()
{}

//Called before render is available
bool EntityManager::Awake(pugi::xml_node& a)
{
	active = true;

	return true;
}

// Called before the first frame
bool EntityManager::Start()
{

	buildingTex = App->tex->Load("assets/buildings/GENHouse.png");

	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			it._Ptr->_Myval->Start();
		}
	}
	return true;
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

	if (selectedUnit) 
	{
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			selectedUnit->position.y -= 100.f * dt;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			selectedUnit->position.y += 100.f * dt;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			selectedUnit->position.x += 100.f * dt;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			selectedUnit->position.x -= 100.f * dt;
		}
		App->scene->aabbTree.baseNode.UpdateNodePoints();
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
	entities.clear();

	App->tex->UnLoad(buildingTex);
	//App->tex->UnLoad(entTex);

	return true;
}

void EntityManager::DrawEverything()
{
	float dt = App->GetDT();

	for (unsigned i = 0; i < entities.size(); i++)
	{
		for (std::list<Entity*>::iterator it = entities[(EntityType)i].begin(); it != entities[(EntityType)i].end(); it++)
		{
			it._Ptr->_Myval->Draw(dt);
			//App->render->DrawQuad({(int)(*it)->position.x + App->map->data.tile_width / 2,(int)(*it)->position.y, 3, 3}, 255, 0, 0);
		}
	}
}

Entity* EntityManager::CreateUnitEntity(iPoint pos)
{
	Unit* ret = new Unit();
	ret->Init(pos);

	entities[EntityType::UNIT].push_back(ret);

	//DELETE: THIS
	entities[EntityType::UNIT].sort(entity_Sort());

	App->scene->aabbTree.AddUnitToTree(*ret);
	selectedUnit = ret;

	return ret;
}
Entity* EntityManager::CreateBuildingEntity(iPoint pos)
{

	//TODO 1: Check if the new building is inside an existing one
	bool exit = false;
	for (std::list<Entity*>::iterator it = entities[EntityType::BUILDING].begin(); it != entities[EntityType::BUILDING].end(); it++)
	{
		if (App->map->WorldToMap(pos.x + App->map->data.tile_width / 2, pos.y) == App->map->WorldToMap((*it)->position.x + App->map->data.tile_width / 2, (*it)->position.y))
		{
			exit = true;
			break;
		}
	}
	if (exit)
		return nullptr;

	Building* ret = new Building();
	ret->Init(pos, buildingTex);

	iPoint iso = pos;
	iso += App->map->GetTilesHalfSize();
	iso = App->map->WorldToMap(iso.x, iso.y);

	ret->type = EntityType::BUILDING;

	//TODO load spritesheet when needed only? first call of constructor of entity?
	entities[EntityType::BUILDING].push_back(ret);

	App->scene->quadTree.AddEntityToNode(*ret, {pos.x + App->map->data.tile_width / 2, pos.y});

	entities[EntityType::BUILDING].sort(entity_Sort());

	return ret;
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