#include "CombatUnit.h"
#include "p2Log.h"

CombatUnit::CombatUnit(UnitType type, iPoint pos) : Unit(type, pos), range(0), damage(0)
{
	//TODO 10: Change textures
	unitType = type;
	position = {(float)pos.x, (float)pos.y};
	switch (unitType)
	{
	case UnitType::ASSASSIN:
		time_production = 90;
		time_research = 0;
		researched = true;
		//Change texture

		CombatUnit::Init(100, 15, 1, 80);
		break;
	case UnitType::PIKEMAN:
		time_production = 90;
		time_research = 70;
		researched = false;
		//Change Texture

		CombatUnit::Init(110, 25, 1, 40);
		break;
	case UnitType::EXPLORER:
		break;
	case UnitType::PRIEST:
		break;
	case UnitType::FOOTMAN:
		break;
	}
}

CombatUnit::~CombatUnit()
{
}

void CombatUnit::Action(Entity* entity)
{
	switch (unitType)
	{
	case UnitType::ASSASSIN:
	LOG("I'm a assassin unit!");
		break;
	case UnitType::PIKEMAN:
	LOG("I'm a pikeman unit!");
		break;
	
	}
	//Attack enemy

}




void CombatUnit::Init(int maxHealth, int damage, int range, int speed)
{
	Unit::Init(maxHealth);
	this->damage = damage;
	this->range = range;
	this->speed = speed;
	SetMoveSpeed(speed);

	//TODO: Convert to a get funtions
	targetPosition.ResetAsPosition();
}

bool CombatUnit::Update(float dt) 
{
	Unit::Update(dt);

	return true;
}

int CombatUnit::GetDamageValue()
{
	return damage;
}

int CombatUnit::GetRangeValue()
{
	return range;
}


int CombatUnit::GetSpeedValue()
{
	return speed;
}



void CombatUnit::IncreaseSpeed(int value)
{
	speed += value;
}

void CombatUnit::IncreaseDamage(int value)
{
	damage += value;
}

