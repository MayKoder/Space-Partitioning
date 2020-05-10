#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include<time.h>

#include "j1Module.h"
#include "j1Timer.h"
#include "SDL/include/SDL.h"
#include"p2Point.h"
#include"AABBTree.h"
#include"QuadTree.h"

struct SDL_Rect;
struct SDL_Texture;
class Entity;
class Building;
enum class UnitType;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	int GetRandomIntInRange(int, int);

	SDL_Rect mapLimitsRect;

public:
	SDL_Texture* debugBlue_tex;
	SDL_Texture* debugRed_tex;
	SDL_Texture* fpsText;

	bool paused_game;
	bool godMode;
};

#endif // __j1SCENE_H__