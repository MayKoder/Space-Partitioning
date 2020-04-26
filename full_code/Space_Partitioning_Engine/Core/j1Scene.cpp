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
		App->entityManager->LoadBuildingsBlitRect();

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
	quadTree = new QuadTree(TreeType::ISOMETRIC, position.x + (App->map->data.tile_width / 2), position.y, size.x, size.y);
	//quadTree->baseNode->SubDivide(quadTree->baseNode, 5);

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
	// -------
	//if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
	//	App->LoadGame("save_game.xml");

	//if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
	//	App->SaveGame("save_game.xml");

	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		exitGame = true;


	SDL_Rect correctedCamera = App->render->camera;
	correctedCamera.x = -correctedCamera.x;
	correctedCamera.y = -correctedCamera.y;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		if (correctedCamera.y - floor(1000.0f * dt) >= mapLimitsRect.y)
		{
			App->render->camera.y += floor(1000.0f * dt);
		}
		else
		{
			App->render->camera.y = 0;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		if (correctedCamera.y + App->render->camera.h + floor(1000.0f * dt) <= mapLimitsRect.h)
		{
			App->render->camera.y -= floor(1000.0f * dt);
		}
		else
		{
			App->render->camera.y = -mapLimitsRect.h + App->render->camera.h;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (correctedCamera.x - floor(1000.0f * dt) >= mapLimitsRect.x)
		{
			App->render->camera.x += floor(1000.0f * dt);
		}
		else
		{
			App->render->camera.x = -mapLimitsRect.x;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if (correctedCamera.x + App->render->camera.w + floor(1000.0f * dt) <= mapLimitsRect.x + mapLimitsRect.w)
		{
			App->render->camera.x -= floor(1000.0f * dt);
		}
		else
		{
			App->render->camera.x = -(mapLimitsRect.x + mapLimitsRect.w) + App->render->camera.w;
		}
	}


	App->map->Draw();


	//Quad draw
	App->render->DrawQuadTree(quadTree->type, quadTree->baseNode);

	iPoint p = App->map->GetMousePositionOnMap();
	if (!App->entityManager->crPreview.active && IN_RANGE(p.x, 0, App->map->data.width-1) == 1 && IN_RANGE(p.y, 0, App->map->data.height-1) == 1)
	{
		p = App->map->MapToWorld(p.x, p.y);
		App->render->Blit(debugBlue_tex, p.x, p.y);
	}

	//App->render->DrawQuad(mapLimitsRect, 255, 255, 255, 40);

	/*CheckSpatial Audio
	if (App->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {
		Mix_HaltChannel(-1);
		Mix_SetPosition(2, 270, 200);
		App->audio->PlayFx(2, WinGreek_sound, 1);
	}
	if (App->input->GetKey(SDL_SCANCODE_O) == KEY_DOWN) {
		Mix_HaltChannel(-1);
		Mix_SetPosition(3, 90, 1);
		App->audio->PlayFx(3, WinGreek_sound, 1);
	}*/

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate()
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");


	App->tex->UnLoad(debugBlue_tex);
	App->tex->UnLoad(debugRed_tex);

	//quadTree->Clear();

	return true;
}

// Called when returning to main menu (either winning/losing or by menu options like exit)
void j1Scene::BackToTitleMenu() {
	destroy = true;
	App->map->destroy = true;
	App->entityManager->destroy = true;
	//App->fade_to_black->FadeToBlack(which_fade::scene_to_title, 2);

	//App->change_scene = true;
}

// Called when restarting the game
void j1Scene::RestartGame() {
	App->restart_scene = true;
	destroy = true;
	App->map->destroy = true;
	App->entityManager->destroy = true;
}

void j1Scene::OnClick(UI* element, float argument)
{

	switch (element->type)
	{
	}
}