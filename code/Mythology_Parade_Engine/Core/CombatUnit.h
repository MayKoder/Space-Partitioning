#ifndef __COMBATUNIT_H__
#define __COMBATUNIT_H__

#include "Unit.h"


class CombatUnit :	public Unit
{
private:
	int damage;
	int range;
	int speed;

public:
	CombatUnit(UnitType, iPoint);
	~CombatUnit();

	bool Update(float);
	void Action(Entity*) override;
	void LevelUp();
private:
	void Init(int maxHealth, int damage, int range, int speed);

public:
	int GetDamageValue();
	int GetRangeValue();
	int GetSpeedValue();
	void IncreaseHealth(int);
	void IncreaseSpeed(int);
	void IncreaseDamage(int);

};

#endif // !__COMBATUNIT_H__
