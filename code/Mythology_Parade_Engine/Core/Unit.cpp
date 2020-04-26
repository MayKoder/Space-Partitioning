#include "Unit.h"
#include "p2Log.h"
#include "j1Textures.h"
#include "j1Input.h"
#include"CombatUnit.h"

Unit::Unit(UnitType type, iPoint pos): unitType(type), _isSelected(false), moveSpeed(60)
{
	
	displayDebug = false;

	collisionRect = { 0, 0, 30, -55 };
	unitType = type;
	position = {(float)pos.x, (float)pos.y};

	flipState = SDL_FLIP_NONE;
	directionToTarget = {0, 0};
	normalizedDirection = { 0, 0 };

	timeToDespawn = 1.f;

	//Init Units
	switch (type)
	{
	case UnitType::MONK:
		time_production = 90;
		time_research = 0;
		researched = true;
		Init(1);
		break;
	}

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

void Unit::Init(int maxHealth)
{


	targetPosition.ResetAsPosition();
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

	App->render->Blit(texture, position.x - blitRect.x / 2, position.y - blitRect.y, blitRect, &spriteRect, 1.f, flipState);

	//App->render->DrawQuad({(int)position.x, (int)position.y, 2, 2}, 0, 255, 0);

	if (displayDebug) 
	{
		if (entPath.size() > 0)
		{
			for (uint i = 0; i < entPath.size(); ++i)
			{
				iPoint pos = App->map->MapToWorld(entPath.at(i).x, entPath.at(i).y);
				App->render->Blit(App->scene->debugBlue_tex, pos.x, pos.y);
			}
		}
		App->render->DrawQuad(collisionRect, 0, 255, 0, 50);
	}

	return true;
}

void Unit::Action(Entity* entity)
{

	switch (unitType)
	{
		//Monks action are controled by Monastery so, its functionality is there ( on Monastery Building )
	}
}


void Unit::SetPath(const std::vector<iPoint> s_path)
{
	entPath = s_path;
}

