#ifndef __UNIT_H__
#define __UNIT_H__

#include "Entity.h"
#include "SDL/include/SDL_rect.h"
#include"EntityManager.h"

enum class ReligiousType
{
	MONK,
	CLERIC,
	MISSIONARY
};

enum class UnitType 
{
	PIKEMAN,
	ASSASSIN,
	EXPLORER,
	PRIEST,
	FOOTMAN,
	MONK,
	CLERIC,
	MISSIONARY
};

class Unit : public Entity
{
private:

	//Move Speed
	int moveSpeed;

	//Conditions
	bool _isSelected;

	//Description / Effect
	std::string description;

public: 
	//Unit Type
	UnitType unitType;
	int time_production;
	int time_research;
	bool researched;
	Unit* enemyTarget;

public:
	Unit(UnitType, iPoint);
	virtual ~Unit();

	void Init(int maxHealth);

	bool Start() override;
	bool Update(float dt);

	void SetMoveSpeed(int);

	bool isSelected();

	virtual bool Draw(float dt);
	virtual void Action(Entity*);

	void MoveToTarget();

	void SetPath(const std::vector<iPoint>);

protected:

	iPoint targetPosition;
	iPoint directionToTarget;
	fPoint normalizedDirection;

	std::vector<iPoint> entPath;

	float timeToDespawn;
	//void SetTarget();
	//void CheckState();
	
};

#endif // !__UNIT_H__
