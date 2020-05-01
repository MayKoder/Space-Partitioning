#ifndef _ENTITY_H
#define _ENTITY_H

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"
#include "j1App.h"
#include "j1Render.h"

enum class EntityType
{
	UNIT,
	BUILDING
};

class Entity
{
public:
	Entity(/*EntityTypes type*/) 
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

protected:
	SDL_Rect collisionRect;
};
#endif // !ENTITY_H