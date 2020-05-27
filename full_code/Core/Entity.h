#ifndef _ENTITY_H
#define _ENTITY_H

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"
#include "j1App.h"
#include "j1Render.h"
#include"MaykMath.h"

enum class EntityType
{
	UNIT,
	BUILDING
};

class Entity
{
public:
	Entity(/*EntityTypes type*/) : tex(nullptr)
	{

	}

	// Destructor
	virtual ~Entity() 
	{
		CleanUp();
	}

	virtual void Init(iPoint pos)
	{

	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	virtual bool Draw(float dt) 
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	bool operator == (const Entity& s) const { return this == &s; }
	bool operator != (const Entity& s) const { return !operator==(s); }

public:

	SDL_Texture* tex;
	EntityType type;
	fPoint position;

	//Rect in the spritesheet
	SDL_Rect spriteRect;

	//W and H for the blit
	iPoint blitRect;

	SDL_Rect getCollisionRect()
	{
		return collisionRect;
	}

	Rect getCollisionAsrect()
	{
		return { collisionRect.x, collisionRect.y + (collisionRect.h *2) + 16, collisionRect.w, -collisionRect.h };
	}

	Rect getCollisionMathRect()
	{
		return {collisionRect.x, collisionRect.y + collisionRect.h, collisionRect.w, -collisionRect.h};
	}

	iPoint getMiddlePoint()
	{
		return { collisionRect.x + (collisionRect.w / 2), collisionRect.y + (collisionRect.h / 2)};
	}

protected:
	SDL_Rect collisionRect;
};
#endif // !ENTITY_H