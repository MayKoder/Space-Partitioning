#include "Building.h"
#include "p2Log.h"
Building::Building(BuildingType type, iPoint pos, BuildingInfo info)
{

	position = { (float)pos.x, (float)pos.y};
	time_producing = 0;
	percentage_constructing = 0;
	first_time_constructing = true;
	buildingType = type;

	displayDebug = false;


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

	//Draw();

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
