#include "Unit.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Input.h"

Unit::Unit(): _isSelected(false), moveSpeed(60)
{
	
	collisionRect = { 0, 0, 30, -55 };
	position = {(float)0, (float)0};
	directionToTarget = {0, 0};
	normalizedDirection = { 0, 0 };

	timeToDespawn = 1.f;

}

void Unit::Init(iPoint pos) 
{
	collisionRect = { 0, 0, 30, -55 };
	position = { (float)pos.x, (float)pos.y };
	directionToTarget = { 0, 0 };
	normalizedDirection = { 0, 0 };

	timeToDespawn = 1.f;
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

void Unit::MoveToTarget()
{
	//if (!isSelected())
	//	return;
	float speed = moveSpeed * App->GetDT();

	//Fast fix for ft increasing bug
	if (App->GetDT() >= 0.5f) 
	{
		speed = 0.f;
	}

	iPoint targetIso = App->map->MapToWorld(targetPosition.x, targetPosition.y);
	targetIso += App->map->GetTilesHalfSize();

	//App->render->DrawLine(position.x, position.y, targetIso.x, targetIso.y, 255, 0, 0);

	fPoint cast = { (float)targetIso.x, (float)targetIso.y };

	fPoint increment = { normalizedDirection.x * speed,  normalizedDirection.y * speed };

	position = position + increment;

	//state = AnimationType::WALK;

	if (position.DistanceManhattan(cast) <= 5)
	{
		position = App->map->MapToWorld((float)targetPosition.x, (float)targetPosition.y);
		position += App->map->GetTilesHalfSizeFloat();

		targetPosition.ResetAsPosition();
	}
	
}


bool Unit::Draw(float dt)
{
	if (entPath.size() > 0 && targetPosition == iPoint(-1, -1))
	{
		targetPosition.x = entPath[0].x;
		targetPosition.y = entPath[0].y;

		iPoint rest = {(int)position.x, (int)position.y};

		iPoint fTarget = App->map->MapToWorld(targetPosition.x, targetPosition.y);
		fTarget += App->map->GetTilesHalfSize();

		directionToTarget = fTarget - rest;
		normalizedDirection = fPoint::Normalize((fPoint)directionToTarget);

		entPath.erase(entPath.begin(), entPath.begin() + 1);
	}


	if (targetPosition != iPoint(-1, -1))
		MoveToTarget();

	//Collider update
	collisionRect.x = position.x - (collisionRect.w / 2);
	collisionRect.y = position.y;

	App->render->DrawQuad({ (int)position.x, (int)position.y, blitRect.x, blitRect.y }, 255, 255, 255);

	return true;
}


void Unit::SetPath(const std::vector<iPoint> s_path)
{
	entPath = s_path;
}

