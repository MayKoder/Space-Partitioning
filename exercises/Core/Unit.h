#ifndef __UNIT_H__
#define __UNIT_H__

#include "Entity.h"
#include "SDL/include/SDL_rect.h"
#include"MaykMath.h"

class Unit : public Entity
{

public:
	Unit();
	virtual ~Unit();

	void Init(iPoint);
	bool Update(float dt);

	virtual bool Draw(float dt);
	
};

#endif // !__UNIT_H__
