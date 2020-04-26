#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"
#include "j1Timer.h"
#include "SDL/include/SDL.h"
#include"p2Point.h"

struct SDL_Rect;

enum class CloseSceneMenus {
	None,
	Pause,
	Options,
	Confirmation,
	Confirmation_and_Pause,
	Research,
	Unknown
};

struct SDL_Texture;
class QuadTree;
class Entity;
class Building;
enum class UnitType;
enum BuildingType;
enum CivilizationType;

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

	// Called when returning to main menu (either winning/losing or by menu options like exit)
	void BackToTitleMenu();

	// Called when restarting the game
	void RestartGame();

	// Called to get the rect of the sprite of the portrait
	SDL_Rect GetSpritePortrait(int type_of_portrait, UnitType unit_type);

	void OnClick(UI* element, float argument = 0);

private:
	enum class Type_Selected {
		None,
		Assassin,
		Pikeman,
		Monk,
		Priest,
		Fortress,
		Temple,
		Encampment,
		Monastery,
		Unknown
	};

	SDL_Rect mapLimitsRect;

public:
	SDL_Texture* debugBlue_tex;
	SDL_Texture* debugRed_tex;
	bool paused_game;
	bool godMode;

	QuadTree* quadTree;
};

#endif // __j1SCENE_H__