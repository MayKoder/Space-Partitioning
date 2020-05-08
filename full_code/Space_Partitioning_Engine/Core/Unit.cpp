#include "Unit.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Input.h"

Unit::Unit()
{
	collisionRect = { 0, 0, 30, -55 };
	position = {(float)0, (float)0};
}

void Unit::Init(iPoint pos) 
{
	position = { (float)pos.x + 20, (float)pos.y };
	blitRect = { 32, 24 };
	collisionRect = { (int)position.x, (int)position.y, blitRect.x, -blitRect.y};
}

Unit::~Unit()
{
}

bool Unit::Update(float dt)
{
	bool ret = true;

	collisionRect.x = (int)position.x;
	collisionRect.y = (int)position.y;

	//Return
	return ret;
}

bool Unit::Draw(float dt)
{
	//App->render->DrawQuad({(int)position.x, (int)position.y, blitRect.x, -blitRect.y}, 255, 255, 255);
	App->render->Blit(tex, (int)position.x, (int)position.y - blitRect.y, blitRect);
	//App->render->DrawQuad(collisionRect, 255, 0, 0, 50);
	return true;
}