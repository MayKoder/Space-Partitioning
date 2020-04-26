#ifndef __CONSOLE_H__
#define __CONSOLE_H__

#include "j1Module.h"
#include "SDL/include/SDL_rect.h"

class ImageUI;
class TextInputUI;
class ListTextsUI;

enum class commands {
	none,
	list,
	god_mode,
	quit,
	FPS,
	map,
	fullscreen
};

class Console :public j1Module {
public:
	Console();

	// Destructor
	virtual ~Console() {};

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt) { return true; };
	bool PostUpdate() { return true; };

	void ActivateConsole();
	void CloseConsole();

	// Called before quitting
	bool CleanUp() { return true; };

	std::string CheckCommand();
	void ExecuteCommand(std::string argument);

public:
	ListTextsUI* console_log;
	ImageUI* console_background;
	TextInputUI* console_input;
	bool console_active;
private:
	SDL_Rect background_pos;
	int background_red;
	int background_green;
	int background_blue;
	int background_alpha;
	SDL_Rect input_pos;
	int input_red;
	int input_green;
	int input_blue;
	int input_alpha;
	SDL_Rect output_pos;
	SDL_Rect output_drag_area;
	std::string label;
	commands command;
};

#endif