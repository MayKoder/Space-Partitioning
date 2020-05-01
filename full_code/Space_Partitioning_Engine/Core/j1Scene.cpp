#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "EntityManager.h"
#include "j1Scene.h"

#include"QuadTree.h"
#include"MaykMath.h"

j1Scene::j1Scene() : j1Module()
{
	name.append("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

// Called before the first frame
bool j1Scene::Start()
{

	if (App->map->Load("MainMap.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		App->map->CreateWalkabilityMap(w, h, &data);

		mapLimitsRect = App->map->GetMapRect();

		RELEASE_ARRAY(data);
	}

  //Load building debug textures
	debugBlue_tex = App->tex->Load("maps/path2.png");
	debugRed_tex = App->tex->Load("maps/cantBuild.png");

	paused_game = false;

	iPoint position;
	iPoint size;
	position = App->map->WorldToMap(0, 0);
	size = iPoint(App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height);
	quadTree.Init(TreeType::ISOMETRIC, position.x + (App->map->data.tile_width / 2), position.y, size.x, size.y);




	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	
	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		exitGame = true;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		App->render->camera.y += floor(1000.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		App->render->camera.y -= floor(1000.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		App->render->camera.x += floor(1000.0f * dt);
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		App->render->camera.x -= floor(1000.0f * dt);
	}


	App->map->Draw();

	App->entityManager->DrawEverything();
	//Quad draw
	App->render->DrawQuadTree(quadTree.type, quadTree.baseNode);
	App->render->DrawAABBTree(aabbTree.type, aabbTree.baseNode);


	iPoint p = App->map->GetMousePositionOnMap();
	if (IN_RANGE(p.x, 0, App->map->data.width-1) == 1 && IN_RANGE(p.y, 0, App->map->data.height-1) == 1)
	{
		if (App->input->GetMouseButtonDown(1) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateBuildingEntity(mouse);
		}

		if (App->input->GetMouseButtonDown(3) == KEY_DOWN)
		{
			iPoint mouse = App->map->GetMousePositionOnMap();
			mouse = App->map->MapToWorld(mouse.x, mouse.y);
			mouse.y += App->map->data.tile_height / 2;

			App->entityManager->CreateUnitEntity(mouse);
		}


		p = App->map->MapToWorld(p.x, p.y);
		App->render->Blit(debugBlue_tex, p.x, p.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	//std::string	lmao = std::to_string((App->GetDT() * 10));

	//fpsText = App->font->Print(lmao.c_str(), { 255, 255, 255, 255 });
	//App->render->Blit(fpsText, 0, 0, NULL, 0.f);

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");


	App->tex->UnLoad(debugBlue_tex);
	App->tex->UnLoad(debugRed_tex);
	//SDL_DestroyTexture(fpsText);

	//quadTree->Clear();

	return true;
}

void j1Scene::OnClick(UI* element, float argument)
{

	switch (element->type)
	{
	}
}