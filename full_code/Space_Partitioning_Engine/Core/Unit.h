#ifndef __UNIT_H__
#define __UNIT_H__

#include "Entity.h"
#include "SDL/include/SDL_rect.h"
#include"MaykMath.h"

class Unit : public Entity
{
private:

	//Move Speed
	int moveSpeed;
	//Conditions
	bool _isSelected;
	bool isDead;

public:
	Unit();
	virtual ~Unit();

	void Init(iPoint);

	bool Start() override;
	bool Update(float dt);

	void SetMoveSpeed(int);

	bool isSelected();

	virtual bool Draw(float dt);
	
};

#endif // !__UNIT_H__
