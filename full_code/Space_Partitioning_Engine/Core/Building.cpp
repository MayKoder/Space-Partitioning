#include "Building.h"
#include "p2Log.h"
Building::Building()
{
	position = { (float)0, (float)0};

	original_spriteRect = spriteRect = {0, 0, 0, 0};
	blitRect = {0, 0};


	//buildingType = info.buildingType;
	tileLenght = 0;

	collisionRect = { (int)position.x, (int)position.y + ((App->map->data.tile_height / 2) * tileLenght), blitRect.x, -blitRect.y};
}

void Building::Init(iPoint pos)
{
	position = { (float)pos.x, (float)pos.y };

	original_spriteRect = spriteRect = {0, 0, 0, 0};
	blitRect = {64, 64};

	tileLenght = 4;

	collisionRect = { (int)position.x, (int)position.y + ((App->map->data.tile_height / 2) * tileLenght), blitRect.x, -blitRect.y };
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
	int blitWidth = tileLenght * App->map->data.tile_width;

	//Draw();

	return ret;
}

bool Building::Draw(float dt)
{
	App->render->DrawQuad({(int)position.x, (int)position.y, blitRect.x, -blitRect.y}, 255, 255, 255);
	return true;
}
