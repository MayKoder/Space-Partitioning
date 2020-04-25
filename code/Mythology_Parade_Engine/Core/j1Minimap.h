#ifndef _j1MINIMAP_H
#define _j1MINIMAP_H
#include "j1Module.h"
#include "p2Point.h"
#include "SDL/include/SDL.h"

enum class Corner {
	TOP_LEFT,
	TOP_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_RIGHT
};


class j1Minimap : public j1Module {
public:
	j1Minimap();
	~j1Minimap();

	bool Start();
	bool Awake(pugi::xml_node& config);
	bool PostUpdate();
	bool CleanUp();

	bool CreateMinimap();
	iPoint WorldToMinimap(int x, int y);
	iPoint ScreenToMinimapToWorld(int x, int y);

public:
	iPoint position;
	int width;
	int height;
	float scale;
	SDL_Texture* texture;
private:
	int map_width;
	int map_height;
	int margin;
	Corner corner;
	SDL_Rect minimap_test_rect;
};

#endif // !_j1MINIMAP_H