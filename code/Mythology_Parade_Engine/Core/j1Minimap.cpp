#include "j1Minimap.h"
#include "j1App.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Render.h"
#include "j1Map.h"
#include "p2Log.h"

j1Minimap::j1Minimap() : j1Module() {
	name.append("minimap");

	texture = nullptr;
	minimap_test_rect = { 0,0,4,4 };
}

j1Minimap::~j1Minimap() {
}

bool j1Minimap::Awake(pugi::xml_node& config) {
	active = false;
	
	uint window_width, window_height;

	//width
	width = config.child("measurements").attribute("width").as_int();

	//corner
	std::string corner_string(config.child("measurements").attribute("corner").as_string());
	margin = config.child("measurements").attribute("margin").as_int();

	if (corner_string == "top_left") {
		corner = Corner::TOP_LEFT;
	}
	else if (corner_string == "top_right") {
		corner = Corner::TOP_RIGHT;
	}
	else if (corner_string == "bottom_left") {
		corner = Corner::BOTTOM_LEFT;
	}
	else if (corner_string == "bottom_right") {
		corner = Corner::BOTTOM_RIGHT;
	}

	return true;
}

bool j1Minimap::Start() {
	bool ret = true;
	uint window_width, window_height;
	App->win->GetWindowSize(window_width, window_height);

	map_width = App->map->data.tile_width * App->map->data.width;
	map_height = App->map->data.tile_height * App->map->data.height;
	scale = ((width) / ((float)map_width));
	height = (map_height)*scale;

	texture = SDL_CreateTexture(App->render->renderer, SDL_GetWindowPixelFormat(App->win->window), SDL_TEXTUREACCESS_TARGET, 1.05f * width, 1.05f * height);
	SDL_SetRenderTarget(App->render->renderer, texture);
	CreateMinimap();
	SDL_SetRenderTarget(App->render->renderer, NULL);

	//calculate position depending on the chosen corner
	switch (corner)
	{
	case Corner::TOP_LEFT:
		position.x = margin;
		position.y = margin;
		break;
	case Corner::TOP_RIGHT:
		position.x = window_width - width - margin;
		position.y = margin;
		break;
	case Corner::BOTTOM_LEFT:
		position.x = margin;
		position.y = window_height - height - margin;
		break;
	case Corner::BOTTOM_RIGHT:
		position.x = window_width - width - margin;
		position.y = window_height - height - margin;
		break;
	}

	return ret;
}

bool j1Minimap::PostUpdate() {

	App->render->Blit(texture, position.x, position.y, NULL, 0.0, 0);

	return true;
}

bool j1Minimap::CreateMinimap() {

	bool ret = true;
	for (std::list<MapLayer*>::iterator it = App->map->data.downLayers.begin(); ret == true && it != App->map->data.downLayers.end(); it++)
	{
		MapLayer* layer = it._Ptr->_Myval;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		int half_width = map_width * 0.5f;

		for (int y = 0; y < App->map->data.height; ++y)
		{
			for (int x = 0; x < App->map->data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = App->map->MapToWorld(x, y);
					App->render->Blit(tileset->texture, pos.x + half_width, pos.y, &r, 0.0f, (0.0), 2147483647, 2147483647, scale);
				}
			}
		}
	}

	for (std::list<MapLayer*>::iterator it = App->map->data.topLayers.begin(); ret == true && it != App->map->data.topLayers.end(); it++)
	{
		MapLayer* layer = it._Ptr->_Myval;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		int half_width = map_width * 0.5f;

		for (int y = 0; y < App->map->data.height; ++y)
		{
			for (int x = 0; x < App->map->data.width; ++x)
			{
				int tile_id = layer->Get(x, y);
				if (tile_id > 0)
				{
					TileSet* tileset = App->map->GetTilesetFromTileId(tile_id);

					SDL_Rect r = tileset->GetTileRect(tile_id);
					iPoint pos = App->map->MapToWorld(x, y);
					App->render->Blit(tileset->texture, pos.x + half_width, pos.y, &r, 0.0f, (0.0), 2147483647, 2147483647, scale);
				}
			}
		}
	}
	
	return true;
}

bool j1Minimap::CleanUp() 
{
	SDL_DestroyTexture(texture);
	return true;
}

iPoint j1Minimap::WorldToMinimap(int x, int y) {
	iPoint minimap_position;
	minimap_position.x = position.x + width * 0.5f + x * scale;
	minimap_position.y = position.y + y * scale;

	return minimap_position;
}

iPoint j1Minimap::ScreenToMinimapToWorld(int x, int y) {
	iPoint minimap_position;
	minimap_position.x = (x - position.x - width * 0.5f) / scale;
	minimap_position.y = (y - position.y) / scale;

	return minimap_position;
}