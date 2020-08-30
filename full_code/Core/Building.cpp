#include "Building.h"
#include "p2Log.h"
Building::Building() : tileLenght(0)
{
	position = { (float)0, (float)0};

	spriteRect = {0, 0, 0, 0};
	blitRect = {0, 0};
	range = 20.f;

	collisionRect = { (int)position.x, (int)position.y + ((App->map->data.tile_height / 2) * tileLenght), blitRect.x, -blitRect.y};
}

void Building::Init(iPoint pos, SDL_Texture* s_tex)
{
	position = { (float)pos.x, (float)pos.y };

	spriteRect = {0, 0, 0, 0};
	blitRect = {64, 64};
	tex = s_tex;

	tileLenght = 4;

	collisionRect = { (int)position.x, (int)position.y + ((App->map->data.tile_height / 2) * tileLenght), blitRect.x, -blitRect.y };
}

Building::~Building()
{
}

bool Building::Draw(float dt)
{
	App->render->Blit(tex, (int)position.x, (int)position.y - blitRect.y + 16, blitRect);
	if (range != 0.f) 
	{
		App->render->DrawCircle(position.x + (collisionRect.w / 2), position.y, range, 0, 255, 0);
	}
	return true;
}
