#include "EntityManager.h"
#include "Building.h"
#include "j1Gui.h"
#include "Unit.h"

#include "p2Log.h"
EntityManager::EntityManager()
{
	name.append("entity_manager");
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

	active = false;

	return true;
}

// Called before the first frame
bool EntityManager::Start()
{
	//TODO: NO HARDCODE BOY
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

	//TODO: Move this logic to the player
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		EnterBuildMode();
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
	entities.clear();

	return true;
}

Entity* EntityManager::CreateUnitEntity(UnitType type, iPoint pos, CivilizationType civilization)
{
	//Unit ret;
	//ret.Init(pos);

	//entities[EntityType::UNIT].push_back(ret);

	////DELETE: THIS
	//entities[EntityType::UNIT].sort(entity_Sort());

	//return &ret;
	return nullptr;
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

Entity* EntityManager::CreateBuildingEntity(iPoint pos)
{
	Building* ret = new Building();
	ret->Init(pos);

	iPoint iso = pos;
	iso += App->map->GetTilesHalfSize();
	iso = App->map->WorldToMap(iso.x, iso.y);

	ret->type = EntityType::BUILDING;

	//TODO load spritesheet when needed only? first call of constructor of entity?
	entities[EntityType::BUILDING].push_back(ret);

	App->scene->quadTree.AddEntityToNode(*ret, {pos.x, pos.y});

	entities[EntityType::BUILDING].sort(entity_Sort());

	return ret;
}

void EntityManager::EnterBuildMode()
{
	crPreview.active = !crPreview.active;
}
void EntityManager::SetBuildIndex(int index)
{
	if (index < MAX_BUILDING_TYPES - 1) 
	{
		buildingTestIndex = index;
	}
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