#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Gui.h"
#include "j1Fonts.h"
#include "EntityManager.h"
#include "j1Minimap.h"
#include "CombatUnit.h"
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
		if (App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		mapLimitsRect = App->map->GetMapRect();
		App->pathfinding->maxPathLenght = App->map->GetMapMaxLenght();
		App->entityManager->LoadBuildingsBlitRect();

		RELEASE_ARRAY(data);
	}

	App->render->camera.x = -2683;
	App->render->camera.y = -2000;


  //Load building debug textures
	debugBlue_tex = App->tex->Load("maps/path2.png");
	debugRed_tex = App->tex->Load("maps/cantBuild.png");

	App->audio->CleanFxs();

	App->gui->sfx_UI[(int)UI_Audio::SAVE] = App->audio->LoadFx("audio/ui/Save.wav");
	App->gui->sfx_UI[(int)UI_Audio::LOAD] = App->audio->LoadFx("audio/ui/load.wav");
	App->gui->sfx_UI[(int)UI_Audio::CONFIRMATION] = App->audio->LoadFx("audio/ui/Click_Standard2.wav");
	App->gui->sfx_UI[(int)UI_Audio::OPTIONS] = App->audio->LoadFx("audio/ui/Settings_Click.wav");
	App->gui->sfx_UI[(int)UI_Audio::RESTART] = App->audio->LoadFx("audio/ui/Restart.wav");
	App->gui->sfx_UI[(int)UI_Audio::SURRENDER] = App->audio->LoadFx("audio/ui/Surrender.wav");
	App->gui->sfx_UI[(int)UI_Audio::EXIT] = App->audio->LoadFx("audio/ui/Exit.wav");
	App->gui->sfx_UI[(int)UI_Audio::CLOSE] = App->audio->LoadFx("audio/ui/Close_Menu.wav");


	close_menus = CloseSceneMenus::None;

	paused_game = false;


	//iPoint position;
	//iPoint size;
	//position = App->map->WorldToMap(0, 0);
	//size = iPoint(App->map->data.width * App->map->data.tile_width, App->map->data.height * App->map->data.tile_height);
	//quadTree = new QuadTree(TreeType::ISOMETRIC, position.x + (App->map->data.tile_width / 2), position.y, size.x, size.y);
	//quadTree->baseNode->SubDivide(quadTree->baseNode, 5);

	//Creating players
	App->entityManager->CreatePlayerEntity();

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate()
{
	// debug pathfing ------------------
	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		//TMP: Temporal pathfinding debug
		std::list<Entity*> list = App->entityManager->getPlayer()->GetEntitiesSelected();

		if(list.size() > 0)
		{
			float n = App->entityManager->getPlayer()->GetEntitiesSelected().size();
			float x = 0, y = 0;

			for (std::list<Entity*>::iterator it = list.begin(); it != list.end(); it++)
			{
				x += it._Ptr->_Myval->position.x;
				y += it._Ptr->_Myval->position.y;
			}

			x /= n;
			y /= n;

			iPoint origin = App->map->WorldToMap((int)x, (int)y);
			iPoint ending = App->map->GetMousePositionOnMap();


			int posX, posY;
			App->input->GetMousePosition(posX, posY);
			iPoint p = App->render->ScreenToWorld(posX, posY);
			p = App->render->ScreenToWorld(posX, posY);

			CivilizationType playerCiv = App->entityManager->getPlayer()->civilization;
			bool attacking = false;

			for (std::list<Entity*>::iterator it = App->entityManager->entities[EntityType::UNIT].begin(); it != App->entityManager->entities[EntityType::UNIT].end(); it++)
			{
				SDL_Rect collider = it._Ptr->_Myval->getCollisionRect();
				if (it._Ptr->_Myval->civilization != playerCiv && EntityManager::IsPointInsideQuad(collider, p.x, p.y))
				{
					Unit* unt = nullptr;
					for (std::list<Entity*>::iterator sel = list.begin(); sel != list.end(); sel++)
					{
						unt = (Unit*)sel._Ptr->_Myval;
						unt->enemyTarget = (Unit*)it._Ptr->_Myval;
						attacking = true;
					}
				}
			}

			if (!attacking)
			{
				Unit* unt = nullptr;
				for (std::list<Entity*>::iterator sel = list.begin(); sel != list.end(); sel++)
				{
					unt = (Unit*)sel._Ptr->_Myval;
					unt->enemyTarget = nullptr;
				}
			}

			if (origin != ending)
				App->pathfinding->RequestPath(origin, ending, list);

		}

	}


	// Move Camera if click on the minimap
	int mouse_x, mouse_y;
	if (((App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN) || (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT)) && paused_game == false)
	{
		App->input->GetMousePosition(mouse_x, mouse_y);
		SDL_Rect minimap = { App->minimap->position.x +15, App->minimap->position.y + 7, App->minimap->width - 28, App->minimap->height -15};

		if ((mouse_x > minimap.x) && (mouse_x < minimap.x + minimap.w) && (mouse_y > minimap.y) && (mouse_y < minimap.y + minimap.h))
		{
			iPoint minimap_mouse_position = App->minimap->ScreenToMinimapToWorld(mouse_x, mouse_y);
			App->render->camera.x = -(minimap_mouse_position.x - App->render->camera.w * 0.5f);
			App->render->camera.y = -(minimap_mouse_position.y - App->render->camera.h * 0.5f);
		}
	}

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

	//if (App->input->drawDebug)
	//	App->render->DrawQuadTree(quadTree->type, quadTree->baseNode);

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
	App->pathfinding->destroy = true;
	App->entityManager->destroy = true;
	App->minimap->destroy = true;
	//App->fade_to_black->FadeToBlack(which_fade::scene_to_title, 2);

	//App->change_scene = true;
}

// Called when restarting the game
void j1Scene::RestartGame() {
	App->restart_scene = true;
	destroy = true;
	App->map->destroy = true;
	App->pathfinding->destroy = true;
	App->entityManager->destroy = true;
	App->minimap->destroy = true;
}

// Called to get the rect of the sprite of the portrait
SDL_Rect j1Scene::GetSpritePortrait(int type_of_portrait, UnitType unit_type) {
	SDL_Rect sprite = { 0,0,0,0 };
	if (type_of_portrait == 0) {
		switch (unit_type) {
		case UnitType::ASSASSIN:
			sprite = { 299,194,30,41 };
			break;
		case UnitType::PIKEMAN:
			sprite = { 331,194,30,41 };
			break;
		case UnitType::MONK:
			sprite = { 362,194,30,41 };
			break;
		case UnitType::PRIEST:
			sprite = { 393,194,30,41 };
			break;
		}
	}
	else if (type_of_portrait == 1) {
		switch (unit_type) {
		case UnitType::ASSASSIN:
			sprite = { 2,321,76,105 };
			break;
		case UnitType::PIKEMAN:
			sprite = { 82,321,76,105 };
			break;
		case UnitType::MONK:
			sprite = { 162,321,76,105 };
			break;
		case UnitType::PRIEST:
			sprite = { 242,321,76,105 };
			break;
		}
	}
	return sprite;

}

// Called to get the rect of the sprite of the portrait of the building
SDL_Rect j1Scene::GetSpritePortraitBuilding(int type_of_portrait, BuildingType building_type, CivilizationType civilization) {
	SDL_Rect sprite = { 0,0,0,0 };
	if (type_of_portrait == 0) {
		switch (building_type) {
		case BuildingType::FORTRESS:
			if (civilization == CivilizationType::VIKING)
				sprite = { 2,431,76,105 };
			else if (civilization == CivilizationType::GREEK)
				sprite = { 323,431,76,105 };
			break;
		case BuildingType::MONASTERY:
			if (civilization == CivilizationType::VIKING)
				sprite = { 82,431,76,105 };
			else if (civilization == CivilizationType::GREEK)
				sprite = { 403,431,76,105 };
			break;
		case BuildingType::ENCAMPMENT:
			if (civilization == CivilizationType::VIKING)
				sprite = { 162,431,76,105 };
			else if (civilization == CivilizationType::GREEK)
				sprite = { 483,431,76,105 };
			break;
		case BuildingType::TEMPLE:
			if (civilization == CivilizationType::VIKING)
				sprite = { 242,431,76,105 };
			else if (civilization == CivilizationType::GREEK)
				sprite = { 563,431,76,105 };
			break;
		}
	}
	return sprite;
}

void j1Scene::OnClick(UI* element, float argument)
{

	switch (element->type)
	{
	}
}