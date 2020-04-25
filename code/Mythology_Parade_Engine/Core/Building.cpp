#include "Building.h"
#include "p2Log.h"
Building::Building(BuildingType type, iPoint pos, BuildingInfo info)
{

	position = { (float)pos.x, (float)pos.y};
	buildingStatus = CONSTRUCTING;
	buildingAction = NOTHING;
	time_producing = 0;
	percentage_constructing = 0;
	first_time_constructing = true;
	buildingType = type;

	if (App->entityManager->getPlayer())
	{
		displayDebug = App->entityManager->getPlayer()->displayDebug;
	}
	else
	{
		displayDebug = false;
	}

	switch (buildingType)
	{
	case FORTRESS:
		this->buildingType = BuildingType::FORTRESS;
		time_construction =time_research = 0;
		damage = 25;
		defenses = 500;
		influence = 20;
		maxCap = 1;
		description = "I'm a fortress";
		researched = true;
		break;
	case MONASTERY:
		this->buildingType = BuildingType::MONASTERY;
		time_research = 60;
		time_construction = 20;
		damage = 15;
		defenses = 250;
		influence = 10;
		maxCap = 5;
		description = "I'm a monastery";
		App->entityManager->getPlayer()->num_monastery++;
		researched = false;
		break;
	case TEMPLE:
		this->buildingType = BuildingType::TEMPLE;
		time_research = 90;
		time_construction = 150;
		damage = 15;
		defenses = 200;
		influence = 10;
		maxCap = 8;
		description = "I'm a temple";
		App->entityManager->getPlayer()->num_temple++;
		researched = false;
		break;
	case ENCAMPMENT:
		this->buildingType = BuildingType::ENCAMPMENT;
		time_research = 90;
		time_construction = 20;
		damage = 20;
		defenses = 350;
		influence = 10;
		maxCap = 7;
		description = "I'm an encampment";
		App->entityManager->getPlayer()->num_encampment++;
		researched = false;
		break;
	default:
		break;
	}


	original_spriteRect = spriteRect = info.spriteRect;
	blitRect = info.blitSize;


	//buildingType = info.buildingType;
	tileLenght = info.tileLenght;

	collisionRect = { (int)position.x, (int)position.y + ((App->map->data.tile_height / 2) * tileLenght), blitRect.x, -blitRect.y};

	timer_construction.Start();
}

Building::~Building()
{
}

const char* Building::GetDescription()
{
	return description.c_str();
}

void Building::CreateUnit()
{
	switch (buildingType)
	{
	case FORTRESS:
		break;
	case MONASTERY:
		App->entityManager->CreateUnitEntity(UnitType::MONK, { (int)position.x - 30, (int)position.y },civilization);
		if (Mix_Playing(4) == 0) 
    {
			App->entityManager->FxUnits(6, App->entityManager->CreateMonk_sound, position.x, position.y);
		}
		break;
	case TEMPLE:
		break;
	case ENCAMPMENT:
		App->entityManager->CreateUnitEntity(UnitType::ASSASSIN, { (int)position.x - 20, (int)position.y },civilization);
		if(Mix_Playing(4) == 0)
    {
			App->entityManager->FxUnits(7, App->entityManager->CreateAssasin_sound, position.x, position.y);
		}

		break;
	}
}

bool Building::Awake(pugi::xml_node& a)
{


	return true;
}

bool Building::Update(float dt)
{
	bool ret = true;
	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		buildingStatus = DESTROYED;
		first_time_constructing = false;
	}
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_DOWN)
	{
		buildingStatus = CONSTRUCTING;
	}
	if (App->input->GetKey(SDL_SCANCODE_5) == KEY_DOWN)
	{
		buildingStatus = FINISHED;
		first_time_constructing = false;
	}
	int blitWidth = tileLenght * App->map->data.tile_width;
	if(buildingStatus==DESTROYED)
	{
		spriteRect = App->entityManager->destructedSpriteRect;

		blitRect = App->entityManager->CalculateBuildingSize(blitWidth, spriteRect.w, spriteRect.h);
	}
	else if (buildingStatus==CONSTRUCTING)
	{
		spriteRect = App->entityManager->constructorSpriteRect;

		blitRect = App->entityManager->CalculateBuildingSize(blitWidth, spriteRect.w, spriteRect.h);
	}
	else if (buildingStatus==FINISHED)
	{
		spriteRect = original_spriteRect;

		blitRect = App->entityManager->CalculateBuildingSize(blitWidth, spriteRect.w, spriteRect.h);
	}

	//Draw();
	if (buildingStatus == CONSTRUCTING || buildingAction==PRODUCING)
	{
		Draw_Construction_Bar(blitWidth);
	}
	else if (buildingAction == RESEARCHING)
	{
		Draw_Construction_Bar(blitWidth, 2);
	}

	//IF MONASTERY DETECTS NEARBY MONKS,INCREASE FAITH
	if (buildingType == BuildingType::MONASTERY)
	{
		std::list<Entity*> list =  App->entityManager->entities[EntityType::UNIT];
		int count = 0;
		for each (Unit* var in list)
		{
			if (var->unitType == UnitType::MONK) {
				if(position.DistanceManhattan(var->position) < 300)
				count++;
			}
		}
		if (nearbyMonks != count)
		{
			nearbyMonks = count;
		}
	}

	return ret;
}

bool Building::Draw(float dt)
{
	//lengh = 4, lenght is the number of tiles this building uses
	//App->render->DrawQuad({position.x, position.y + (tileHeight /2) * (height + 1), texturewidth, -textureHeight}, 255, 250, 20);
	App->render->Blit(texture, position.x, position.y + ((App->map->data.tile_height / 2) * tileLenght) - blitRect.y, {blitRect.x, blitRect.y}, &spriteRect);

	if (displayDebug)
	{
		App->render->DrawQuad(collisionRect, 255, 0, 0, 50);
	}

	return true;
}

void Building::Draw_Construction_Bar(int blitWidth, int bar_used)
{
	SDL_Rect construction_spriteRect = App->entityManager->construction_bar_back;
	iPoint pos = { (int)position.x + (int)(0.15 * blitWidth), (int)position.y + (int)(((32 / 2) * tileLenght) - 1.25 * blitRect.y) };
	App->render->Blit(texture, pos.x, pos.y, &construction_spriteRect);
	if (bar_used == 0)
		construction_spriteRect = App->entityManager->construction_bar_front;
	else if(bar_used==1)
		construction_spriteRect = App->entityManager->life_bar_front;
	else if(bar_used==2)
		construction_spriteRect = App->entityManager->research_bar_front;
	int sprite_rect_width = percentage_constructing * construction_spriteRect.w;
	App->render->Blit(texture, pos.x + 11, pos.y + 3, { sprite_rect_width, construction_spriteRect.h },
		&construction_spriteRect);
	construction_spriteRect = App->entityManager->construction_bar_empty;
	App->render->Blit(texture, pos.x,pos.y, &construction_spriteRect);
}


void Building::StartProducing(int time, std::string thing_producing) {
	buildingAction = PRODUCING;
	percentage_constructing = 0;
	time_producing = time;
	element_producing = thing_producing;
	timer_construction.Start();
	
}

void Building::FinishProduction(std::string thing_produced)
{
	if (thing_produced == "Victory")
	{
		if (civilization == CivilizationType::VIKING)
		{
			App->entityManager->getPlayer()->player_win = true;
		}
		else
		{
			App->entityManager->getPlayer()->player_lose = true;
		}
	}
	else
	{
	CreateUnit();
	}
}

void Building::StartResearching(int time, std::string thing_producing) {
	buildingAction = RESEARCHING;
	percentage_constructing = 0;
	time_producing = time;
	element_producing = thing_producing;
	timer_construction.Start();
}
