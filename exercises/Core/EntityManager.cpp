#include "EntityManager.h"
#include "Building.h"
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

	//Debug texture loading
	buildingTex = App->tex->Load("assets/buildings/GENHouse.png");
	entTex = App->tex->Load("assets/units/Slime.png");

	//TODO 1.2: Initialize quadTree
	//quadTree.Init(type, x, y...);

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

	//------- INPUTS ---------//
	if (App->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
	{
		//quadTree.displayTree = !quadTree.displayTree;
		//aabbTree.displayTree = !aabbTree.displayTree;
	}

	iPoint p = App->map->GetMousePositionOnMap();
	if (IN_RANGE(p.x, 0, App->map->data.width - 1) == 1 && IN_RANGE(p.y, 0, App->map->data.height - 1) == 1)
	{
		//Create building
		if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateBuildingEntity(mouse);
		}
		//Create unit
		if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
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
		//Fill map with units
		if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) 
		{
			for (int y = 1; y < App->map->data.height + 1; y++)
			{
				for (int x = 0; x < App->map->data.height; x++)
				{
					iPoint pos = App->map->MapToWorld(x, y);
					pos.x += 30;
					pos.y += 5;

					CreateUnitEntity(pos);
					//aabbTree.UpdateAllNodes(aabbTree.baseNode);
				}
			}
		}

	}

	//Select unit
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
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

	//------- Unit movement ---------//
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

	//TODO 3: Make sure to update all the AABBTree nodes


	return true;
}



bool EntityManager::PostUpdate()
{

	j1PerfTimer timer;
	double startTimer = timer.ReadMs();

	//TODO 7: Test collision detection in Debug and Release mode
	//for (std::list<Entity*>::iterator it = entities[EntityType::UNIT].begin(); it != entities[EntityType::UNIT].end(); it++)
	//{

	//	//Get the nodes to check inside this unit position
	//	std::vector<AABBNode*> nodesToCheck;
	//	aabbTree.LoadLeafNodesInsideRect(&aabbTree.baseNode, nodesToCheck, (*it)->getCollisionMathRect());

	//	//Iterate all nodes
	//	for (int i = 0; i < nodesToCheck.size(); i++)
	//	{

	//		//Iterate all data in that node
	//		for (std::list<Entity*>::iterator it2 = nodesToCheck[i]->data.begin(); it2 != nodesToCheck[i]->data.end(); it2++)
	//		{
	//			//Check for collisions
	//			if (it._Ptr->_Myval != it2._Ptr->_Myval && MaykMath::CheckRectCollision((*it)->getCollisionMathRect(), (*it2)->getCollisionMathRect()))
	//			{
	//				//LOG("Unit to unit collision");
	//			}
	//		}
	//	}
	//	//Clear the node vector
	//	nodesToCheck.clear();

	//	//Units can collide with buildings, so we use the QuadTree to check that

	//	//TODO 8: Test unit to building collision
	//	//fPoint points[4];
	//	//points[0] = (*it)->position;
	//	//points[1] = { (*it)->position.x, (*it)->position.y - (*it)->blitRect.y};
	//	//points[2] = { (*it)->position.x + (*it)->blitRect.x, (*it)->position.y};
	//	//points[3] = { (*it)->position.x + (*it)->blitRect.x, (*it)->position.y - (*it)->blitRect.y };

	//	//int checks = 0;

	//	////Iterate all 4 points
	//	//for (int i = 0; i < sizeof(points) / sizeof(fPoint); i++)
	//	//{

	//	//	//Find the lowest node in this point
	//	//	quadTree.FindLowestNodeInPoint(&quadTree.baseNode, points[i]);
	//	//	if (quadTree.lowestNode)
	//	//	{

	//	//		//Check every data element in this node
	//	//		for (std::list<Entity*>::iterator it2 = quadTree.lowestNode->data.begin(); it2 != quadTree.lowestNode->data.end(); it2++)
	//	//		{
	//	//			App->render->DrawLine((int)points[i].x, (int)points[i].y, (int)(*it2)->position.x, (int)(*it2)->position.y, 255, 0, 0);
	//	//			//if ((*it2)->position.DistanceNoSqrt(points[i]) <= 20000)
	//	//			//{

	//	//			//}

	//	//			//Check if they are colliding
	//	//			if (MaykMath::CheckRectCollision((*it)->getCollisionMathRect(), (*it2)->getCollisionAsrect())) 
	//	//			{
	//	//				//LOG("Unit to building collision");
	//	//			}
	//	//			checks++;
	//	//		}

	//	//		quadTree.lowestNode = nullptr;
	//	//	}
	//	//}
	//	//LOG("Building Checks: %i", a);
	//}

	//BruteForce
	//Iterate every entity
	for (std::list<Entity*>::iterator A = entities[EntityType::UNIT].begin(); A != entities[EntityType::UNIT].end(); A++)
	{

		//Check with every other entity
		for (std::list<Entity*>::iterator B = entities[EntityType::UNIT].begin(); B != entities[EntityType::UNIT].end(); B++)
		{
			//Check collisions
			if (A._Ptr->_Myval != B._Ptr->_Myval && MaykMath::CheckRectCollision((*A)->getCollisionMathRect(), (*B)->getCollisionMathRect()))
			{
				LOG("Collision");
			}
		}
	}

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

//TODO 5: Fill the code for CreateUnitEntity() method
Entity* EntityManager::CreateUnitEntity(iPoint pos)
{
	pos.x += 20;

	//Check if pos is free to spawn

	Unit* ret = new Unit();
	ret->Init(pos);
	ret->tex = entTex;

	entities[EntityType::UNIT].push_back(ret);

	entities[EntityType::UNIT].sort(entity_Sort());

	//Add unit to tree here

	selectedUnit = ret;

	LOG("Slime added");

	return ret;
}

//TODO 4: Fill the code for CreateBuildingEntity() method
Entity* EntityManager::CreateBuildingEntity(iPoint pos)
{

	j1PerfTimer timer;
	double startTime = timer.ReadMs();

	//Check if pos is free to spawn on

	Building* ret = new Building();
	ret->Init(pos, buildingTex);

	iPoint iso = pos;
	iso += App->map->GetTilesHalfSize();
	iso = App->map->WorldToMap(iso.x, iso.y);

	ret->type = EntityType::BUILDING;

	entities[EntityType::BUILDING].push_back(ret);

	//Add building to static tree here

	entities[EntityType::BUILDING].sort(entity_Sort());

	LOG("Building added, took %f ms", timer.ReadMs() - startTime);

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