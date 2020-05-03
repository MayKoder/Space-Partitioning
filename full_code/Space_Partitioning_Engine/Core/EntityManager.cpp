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


	iPoint p = App->map->GetMousePositionOnMap();
	if (IN_RANGE(p.x, 0, App->map->data.width - 1) == 1 && IN_RANGE(p.y, 0, App->map->data.height - 1) == 1)
	{
		//Create building
		if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateBuildingEntity(mouse);
		}
		//Select unit
		if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
		{
			iPoint p;
			App->input->GetMousePosition(p.x, p.y);
			p = App->render->ScreenToWorld(p.x, p.y);

			std::list<Entity*>* listData = &App->scene->aabbTree.FindLowestNode(&App->scene->aabbTree.baseNode, p)->data;

			for (std::list<Entity*>::iterator it = listData->begin(); it != listData->end(); it++)
			{
				if (IsPointInsideAxisAlignedRectangle((*it)->getCollisionMathRect(), p)) 
				{
					selectedUnit = (*it);
					break;
				}
			}
		}
		//Create unit
		if (App->input->GetMouseButtonDown(3) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateUnitEntity(mouse);
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
	}
	App->scene->aabbTree.UpdateAllNodes(App->scene->aabbTree.baseNode);

	return true;
}



bool EntityManager::PostUpdate()
{

	j1PerfTimer timer;
	double startTimer = timer.ReadMs();

	//Update all collisions
	for (std::list<Entity*>::iterator it = entities[EntityType::UNIT].begin(); it != entities[EntityType::UNIT].end(); it++)
	{
		for (std::list<Entity*>::iterator it2 = entities[EntityType::UNIT].begin(); it2 != entities[EntityType::UNIT].end(); it2++)
		{
			//Calculte collisions
			if (it._Ptr->_Myval != it2._Ptr->_Myval && CheckRectCollision((*it)->getCollisionMathRect(), (*it2)->getCollisionMathRect()))
			{
				LOG("Collision");
				//Point Ac = (*it)->getCollisionMathRect().GetCentralPoint();
				//Point Bc = (*it2)->getCollisionMathRect().GetCentralPoint();

			}
		}
	}

	//LOG("Time to check collisions between %i units took: %f", entities[EntityType::UNIT].size(), timer.ReadMs() - startTimer);

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

	//TODO 1.1: Check if the new entity is inside an existing one

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

	j1PerfTimer timer;
	double startTime = timer.ReadMs();

	//TODO 1.2: Check if the new building is inside an existing one
	bool exit = false;

	//Figure lowest node out
	App->scene->quadTree.FindLowestNodeInPoint(&App->scene->quadTree.baseNode, {pos.x, pos.y});
	QuadNode* node = App->scene->quadTree.lowestNode;

	for (std::list<Entity*>::iterator it = node->data.begin(); it != node->data.end(); it++)
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

	LOG("Item added, took %f ms", timer.ReadMs() - startTime);

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