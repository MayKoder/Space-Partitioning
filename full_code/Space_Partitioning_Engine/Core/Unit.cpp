#include "Unit.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Input.h"

Unit::Unit(): _isSelected(false), moveSpeed(60), isDead(false)
{
	
	collisionRect = { 0, 0, 30, -55 };
	position = {(float)0, (float)0};

}

void Unit::Init(iPoint pos) 
{
	position = { (float)pos.x + 20, (float)pos.y };
	blitRect = { 16, 30 };
	collisionRect = { (int)position.x, (int)position.y, blitRect.x, -blitRect.y};
}

Unit::~Unit()
{
}

bool Unit::Start()
{
	bool ret = true;

	//pugi::xml_document	character_file;
	//pugi::xml_parse_result result = character_file.load_file("assets/units/Assassin.tmx");
	//pugi::xml_node	character_node = character_file.child("map");
	//std::string image_source("assets/units/");
	//image_source += character_node.child("tileset").child("image").attribute("source").as_string();
	//texture = App->tex->Load(image_source.c_str());
	//position_rect.w = 125;
	//position_rect.h = 112;
	//LOG("%s", image_source.c_str());
	return ret;
}

bool Unit::Update(float dt)
{
	bool ret = true;

	collisionRect.x = (int)position.x;
	collisionRect.y = (int)position.y;
	//Allawys blit the sprite at the end
	//ret = Draw(dt);

	//Return
	return ret;
}

void Unit::SetMoveSpeed(int value)
{
	moveSpeed = value;
}

bool Unit::isSelected()
{
	return _isSelected;
}



bool Unit::Draw(float dt)
{
	App->render->DrawQuad({(int)position.x, (int)position.y, blitRect.x, -blitRect.y}, 255, 255, 255);
	App->render->DrawQuad(collisionRect, 255, 0, 0, 50);
	return true;
}