#include "Player.h"
#include <iostream>

#include "j1Scene.h"
#include "j1Input.h"
#include "j1Gui.h"
#include "EntityManager.h"

Player::Player()
{
	Start();
}

Player::~Player()
{
}

bool Player::Awake()
{
	return true;
}

bool Player::Start()
{

	tick2 = SDL_GetTicks();
	player_win = player_lose = false;


	dontSelect = false;
	num_encampment = num_monastery = num_temple = 0;
	time_production_victory = 10;

	player_type = CivilizationType::VIKING;
	displayDebug = false;
	oneTime = true;

	return true;
}

bool Player::PreUpdate()
{

	if (oneTime)
	{
		InitVikings();
		InitGreek();
		oneTime = false;
	}

	return true;
}

bool Player::Update(float dt)
{
	//if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN && !App->entityManager->crPreview.active)
	//{
	//	//Unit spawn
	//	iPoint mouse = App->map->GetMousePositionOnMap();
	//	iPoint spawnPos = App->map->TileCenterPoint(mouse);

	//	//Todo change assassin for the type of unit
	//	App->entityManager->CreateUnitEntity(UnitType::MONK, spawnPos);
	//}

	//Selection logics and drawing
	if (!App->scene->paused_game)
	{
		SelectionDraw_Logic();
		PlayerInputs();
	}



	return true;
}

bool Player::PostUpdate()
{
	return true;
}

bool Player::CleanUp()
{
	listEntities.clear();
	return true;
}

void Player::SelectionDraw_Logic()
{
	if (!App->input->GetMouseButtonDown(1) == KEY_DOWN)
	{
		App->input->GetMousePosition(preClicked.x, preClicked.y);
		if(preClicked.y>=590)
		{
			dontSelect = true;
			return;
		}
		else
		{
			dontSelect = false;
		}
		preClicked = App->render->ScreenToWorld(preClicked.x, preClicked.y);
	}
	if (!dontSelect)
	{
		if (App->input->GetMouseButtonDown(1) == KEY_REPEAT)
		{
			App->input->GetMousePosition(postClicked.x, postClicked.y);
			if (postClicked.y >= 590)
			{
				postClicked.y = 588;
			}
			postClicked = App->render->ScreenToWorld(postClicked.x, postClicked.y);

			App->render->DrawQuad({ preClicked.x, preClicked.y, postClicked.x - preClicked.x, postClicked.y - preClicked.y }, 255, 255, 255, 255, false);
		}

		if (App->input->GetMouseButtonDown(1) == KEY_UP)
		{
			listEntities.clear();
			buildingSelect = nullptr;
			ClickLogic();
			SeeEntitiesInside();
		}
	}
}

std::list<Entity*> Player::GetEntitiesSelected()
{
	return listEntities;
}

Building* Player::GetSelectedBuild()
{
	return (Building*) buildingSelect;
}

void Player::SeeEntitiesInside()
{
	//ALERT MAYK
	std::list<Entity*>::iterator it = App->entityManager->entities[EntityType::UNIT].begin();
	for (it; it != App->entityManager->entities[EntityType::UNIT].end(); ++it)
	{
		if ((it._Ptr->_Myval->position.x >= preClicked.x && it._Ptr->_Myval->position.x <= postClicked.x) || (it._Ptr->_Myval->position.x <= preClicked.x && it._Ptr->_Myval->position.x >= postClicked.x))
		{
			if ((it._Ptr->_Myval->position.y >= preClicked.y && it._Ptr->_Myval->position.y <= postClicked.y) || it._Ptr->_Myval->position.y <= preClicked.y && it._Ptr->_Myval->position.y >= postClicked.y)
			{
				if (it._Ptr->_Myval->civilization == player_type)
				{
					listEntities.push_back(it._Ptr->_Myval);
				}
			}
		}
	}
}

void Player::PlayerInputs()
{

	//CHANGE ALL ITINERATIONS TO entityInsideCamera LIST
	if (App->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		App->scene->godMode = !App->scene->godMode;
		App->input->drawDebug = !App->input->drawDebug;
	}



	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN && App->scene->godMode)
	{
		iPoint mouse = App->map->GetMousePositionOnMap();
		iPoint spawnPos = App->map->TileCenterPoint(mouse);
		App->entityManager->CreateUnitEntity(UnitType::ASSASSIN, spawnPos,civilization);
	}

	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN && App->scene->godMode)
	{
		iPoint mouse = App->map->GetMousePositionOnMap();
		iPoint spawnPos = App->map->TileCenterPoint(mouse);
		App->entityManager->CreateUnitEntity(UnitType::MONK, spawnPos,civilization);
	}

	if (App->input->GetKey(SDL_SCANCODE_F4) == KEY_DOWN && App->scene->godMode)
	{
		if (!listEntities.empty())
		{
			std::list<Entity*>::iterator it = listEntities.begin();
			for (it; it != listEntities.end(); ++it)
			{
				App->entityManager->DeleteEntity(it._Ptr->_Myval);
			}
			listEntities.clear();
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN && App->scene->godMode)
	{
		player_win = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN && App->scene->godMode)
	{
		player_lose = true;
	}

	if (App->input->GetKey(SDL_SCANCODE_F7) == KEY_DOWN && App->scene->godMode)
	{
		//Display collisions
		displayDebug = !displayDebug;

		for (unsigned int i = 1; i < App->entityManager->entities.size(); i++)
		{
			std::list<Entity*>::iterator it = App->entityManager->entities[(EntityType)i].begin();

			Entity* ent = nullptr;
			for (it; it != App->entityManager->entities[(EntityType)i].end(); ++it)
			{
				ent = it._Ptr->_Myval;
				ent->displayDebug = displayDebug;
			}
		}
	}
}

void Player::ClickLogic()
{
	App->input->GetMousePosition(click.x,click.y);
	click = App->render->ScreenToWorld(click.x, click.y);
	std::list<Entity*>::iterator it = App->entityManager->entities[EntityType::BUILDING].begin();
	for (it; it != App->entityManager->entities[EntityType::BUILDING].end(); ++it)
	{
		if (click.x >= it._Ptr->_Myval->getCollisionRect().x && click.x <= it._Ptr->_Myval->getCollisionRect().x + it._Ptr->_Myval->getCollisionRect().w)
		{
			if (click.y <= it._Ptr->_Myval->getCollisionRect().y && click.y >= it._Ptr->_Myval->getCollisionRect().y + it._Ptr->_Myval->getCollisionRect().h)
			{
				buildingSelect = it._Ptr->_Myval;
			}
		}
	}
	if (listEntities.empty())
	{
		it = App->entityManager->entities[EntityType::UNIT].begin();
		for (it; it != App->entityManager->entities[EntityType::UNIT].end(); ++it)
		{
			if (click.x >= it._Ptr->_Myval->getCollisionRect().x && click.x <= it._Ptr->_Myval->getCollisionRect().x + it._Ptr->_Myval->getCollisionRect().w)
			{
				if (click.y <= it._Ptr->_Myval->getCollisionRect().y && click.y >= it._Ptr->_Myval->getCollisionRect().y + it._Ptr->_Myval->getCollisionRect().h)
				{
					if (it._Ptr->_Myval->civilization == civilization) 
					{
						listEntities.push_back(it._Ptr->_Myval);
						if (preClicked == postClicked)
							return;
					}
				}
			}
		}
	}
}




void Player::InitVikings()
{
	iPoint fortress = { 122,21 };
	fortress = App->map->MapToWorld(fortress.x, fortress.y);
	fortress.x -= App->map->GetTilesHalfSize().x;

	App->entityManager->CreateBuildingEntity(fortress, BuildingType::FORTRESS, App->entityManager->buildingsData[0],CivilizationType::VIKING);

	iPoint monkPos = { 119,26 };
	iPoint assassinPos = { 121,28 };
	monkPos = App->map->MapToWorld(monkPos.x, monkPos.y);
	assassinPos = App->map->MapToWorld(assassinPos.x, assassinPos.y);


	App->entityManager->CreateUnitEntity(UnitType::MONK, monkPos,civilization);
	App->entityManager->CreateUnitEntity(UnitType::ASSASSIN, assassinPos,civilization);

}

void Player::InitGreek()
{
	iPoint fortress = { 102,41 };
	fortress = App->map->MapToWorld(fortress.x, fortress.y);
	fortress.x -= App->map->GetTilesHalfSize().x;

	App->entityManager->CreateBuildingEntity(fortress, BuildingType::FORTRESS, App->entityManager->buildingsData[4],CivilizationType::GREEK);

	iPoint monkPos = { 106,34 };
	iPoint assassinPos = { 109,37 };
	monkPos = App->map->MapToWorld(monkPos.x, monkPos.y);
	assassinPos = App->map->MapToWorld(assassinPos.x, assassinPos.y);


	App->entityManager->CreateUnitEntity(UnitType::MONK, monkPos,CivilizationType::GREEK);
	App->entityManager->CreateUnitEntity(UnitType::ASSASSIN, assassinPos,CivilizationType::GREEK);
}
