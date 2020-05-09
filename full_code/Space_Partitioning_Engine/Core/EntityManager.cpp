#include "EntityManager.h"
#include "Building.h"
#include "j1Gui.h"
#include "Unit.h"
#include"j1Window.h"

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
	entTex = App->tex->Load("assets/units/Slime.png");

	iPoint position;
	iPoint size;
	position = App->map->WorldToMap(0, 0);
	size = iPoint(App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height);
	quadTree.Init(TreeType::ISOMETRIC, position.x + (App->map->data.tile_width / 2), position.y, size.x, size.y);

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

	//Select unit
	if (App->input->GetMouseButtonDown(2) == KEY_DOWN)
	{
		iPoint p;
		App->input->GetMousePosition(p.x, p.y);
		p = App->render->ScreenToWorld(p.x, p.y);

		for (std::list<Entity*>::iterator it = entities[EntityType::UNIT].begin(); it != entities[EntityType::UNIT].end(); it++)
		{
			if (MaykMath::IsPointInsideAxisAlignedRectangle((*it)->getCollisionMathRect(), p))
			{
				selectedUnit = (*it);
				break;
			}
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		quadTree.displayTree = !quadTree.displayTree;
		aabbTree.displayTree = !aabbTree.displayTree;
	}

	if (IN_RANGE(p.x, 0, App->map->data.width - 1) == 1 && IN_RANGE(p.y, 0, App->map->data.height - 1) == 1)
	{
		//Create building with 1
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateBuildingEntity(mouse);
		}
		//Create unit with 2
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);

			mouse.x -= 3;
			mouse.y += App->map->data.tile_height / 2 + 10;

			App->entityManager->CreateUnitEntity(mouse);

			//for (int i = 0; i < 1000; i++)
			//{
			//	int x = App->scene->GetRandomIntInRange(1, App->map->data.width * 3);
			//	int y = App->scene->GetRandomIntInRange(1, App->map->data.height * 3);

			//	iPoint pos = App->map->MapToWorld(x, y);
			//	pos.x += 32;

			//	CreateUnitEntity(pos);
			//	App->scene->aabbTree.UpdateAllNodes(App->scene->aabbTree.baseNode);
			//}
		}
		//Fill map with units wirh 3
		if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) 
		{
			for (int y = 1; y < App->map->data.height + 1; y++)
			{
				for (int x = 0; x < App->map->data.height; x++)
				{
					iPoint pos = App->map->MapToWorld(x, y);
					pos.x += 30;
					pos.y += 5;

					CreateUnitEntity(pos);
					aabbTree.UpdateAllNodes(aabbTree.baseNode);
				}
			}
		}

	}

	//Move units
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
		//if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_DOWN) 
		//{
		//	App->scene->aabbTree.DeleteDataElement(App->scene->aabbTree.baseNode, selectedUnit);
		//	DeleteEntity(selectedUnit);
		//	selectedUnit = nullptr;
		//}
	}

	//Update all tree nodes
	aabbTree.UpdateAllNodes(aabbTree.baseNode);

	return true;
}



bool EntityManager::PostUpdate()
{

	j1PerfTimer timer;
	double startTimer = timer.ReadMs();

	//Solutin
	for (std::list<Entity*>::iterator it = entities[EntityType::UNIT].begin(); it != entities[EntityType::UNIT].end(); it++)
	{
		std::vector<AABBNode*> nodesToCheck;
		aabbTree.LoadLeafNodesInsideRect(&aabbTree.baseNode, nodesToCheck, (*it)->getCollisionMathRect());

		//TIME: This takes almost no time
		for (int i = 0; i < nodesToCheck.size(); i++)
		{
			for (std::list<Entity*>::iterator it2 = nodesToCheck[i]->data.begin(); it2 != nodesToCheck[i]->data.end(); it2++)
			{
				//Calculte collisions
				if (it._Ptr->_Myval != it2._Ptr->_Myval && MaykMath::CheckRectCollision((*it)->getCollisionMathRect(), (*it2)->getCollisionMathRect()))
				{
					//LOG("Collision");
					//Point Ac = (*it)->getCollisionMathRect().GetCentralPoint();
					//Point Bc = (*it2)->getCollisionMathRect().GetCentralPoint();

				}
			}
		}
		nodesToCheck.clear();

		//Check with every building if its close
		fPoint lmao[4];
		lmao[0] = (*it)->position;
		lmao[1] = { (*it)->position.x, (*it)->position.y - (*it)->blitRect.y};
		lmao[2] = { (*it)->position.x + (*it)->blitRect.x, (*it)->position.y};
		lmao[3] = { (*it)->position.x + (*it)->blitRect.x, (*it)->position.y - (*it)->blitRect.y };

		int a = 0;
		for (int i = 0; i < 4; i++)
		{
			quadTree.FindLowestNodeInPoint(&quadTree.baseNode, lmao[i]);

			if (quadTree.lowestNode)
			{
				for (std::list<Entity*>::iterator it2 = quadTree.lowestNode->data.begin(); it2 != quadTree.lowestNode->data.end(); it2++)
				{
					App->render->DrawLine((int)lmao[i].x, (int)lmao[i].y, (int)(*it2)->position.x, (int)(*it2)->position.y, 255, 0, 0);
					//if ((*it2)->position.DistanceNoSqrt(lmao[i]) <= 20000)
					//{

					//}

					if (MaykMath::CheckRectCollision((*it)->getCollisionMathRect(), (*it2)->getCollisionAsrect())) 
					{
						//
					}
					a++;
				}

				quadTree.lowestNode = nullptr;
			}
		}
		//LOG("Building Checks: %i", a);

	}

	////BruteForce
	//for (std::list<Entity*>::iterator A = entities[EntityType::UNIT].begin(); A != entities[EntityType::UNIT].end(); A++)
	//{
	//	for (std::list<Entity*>::iterator B = entities[EntityType::UNIT].begin(); B != entities[EntityType::UNIT].end(); B++)
	//	{
	//		//Calculte collisions
	//		if (A._Ptr->_Myval != B._Ptr->_Myval && MaykMath::CheckRectCollision((*A)->getCollisionMathRect(), (*B)->getCollisionMathRect()))
	//		{
	//			LOG("Collision");
	//			//Point Ac = (*it)->getCollisionMathRect().GetCentralPoint();
	//			//Point Bc = (*it2)->getCollisionMathRect().GetCentralPoint();

	//		}
	//	}
	//}

	//LOG("Debug Mode || Brute Force || Time to check %i entities: %fms", entities[EntityType::UNIT].size(), timer.ReadMs() - startTimer);

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
	App->tex->UnLoad(entTex);

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
		}
	}
}

Entity* EntityManager::CreateUnitEntity(iPoint pos)
{

	//TODO 1.1: Check if the new entity is inside an existing one
	bool exit = false;


	//TODO PER EL MAYK: No itineris tots, busca el lowestNode
	for (std::list<Entity*>::iterator it = entities[EntityType::UNIT].begin(); it != entities[EntityType::UNIT].end(); it++)
	{
		if (App->map->WorldToMap(pos.x, pos.y) == App->map->WorldToMap((*it)->position.x, (*it)->position.y))
		{
			exit = true;
			break;
		}
	}
	if (exit)
		return nullptr;

	Unit* ret = new Unit();
	ret->Init(pos);
	ret->tex = entTex;

	entities[EntityType::UNIT].push_back(ret);

	entities[EntityType::UNIT].sort(entity_Sort());

	aabbTree.AddUnitToTree(*ret);
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
	quadTree.FindLowestNodeInPoint(&quadTree.baseNode, {pos.x, pos.y});
	QuadNode* node = quadTree.lowestNode;

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

	entities[EntityType::BUILDING].push_back(ret);

	quadTree.AddEntityToNode(*ret, {pos.x + App->map->data.tile_width / 2, pos.y});

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