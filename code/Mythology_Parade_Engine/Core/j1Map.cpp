#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Input.h"
#include <math.h>
#include"j1Scene.h"
#include"QuadTree.h"

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.append("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.append(config.child("folder").child_value());
	active = false;

	return ret;
}

void j1Map::Draw()
{
	//j1PerfTimer timer;

	//double startTime = timer.ReadMs();

	if (map_loaded == false)
		return;

	iPoint A = WorldToMap(-App->render->camera.x, -App->render->camera.y);
	iPoint B = WorldToMap(-App->render->camera.x + App->render->camera.w, -App->render->camera.y + App->render->camera.h);

	//int blits = 0;
	for (std::list<MapLayer*>::iterator it = data.downLayers.begin(); it != data.downLayers.end(); it++)
	{
		MapLayer* layer = it._Ptr->_Myval;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		//Approach 2.0
		for (int a = A.x+A.y - 2; a <= B.x + B.y - 5 /* or 2*/; a++) 
		{
			for (int b = A.x - A.y - 2; b <= B.x - B.y +2; b++) 
			{
				if ((b & 1) != (a & 1)) 
					continue;

				int x = (a + b) / 2;
				int y = (a - b) / 2;

				if (x >= 0 && y >= 0 && x < data.width && y < data.height) 
				{
					int tile_id = layer->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						App->render->Blit(tileset->texture, pos.x, pos.y, &r);

						//blits++;
					}
				}

			}
		}
	}

	App->entityManager->DrawEverything();

	for (std::list<MapLayer*>::iterator it = data.topLayers.begin(); it != data.topLayers.end(); it++)
	{
		MapLayer* layer = it._Ptr->_Myval;

		if (layer->properties.Get("Nodraw") != 0)
			continue;

		//Approach 2.0
		for (int a = A.x + A.y - 2; a <= B.x + B.y - 5 /* or 2*/; a++)
		{
			for (int b = A.x - A.y - 2; b <= B.x - B.y + 2; b++)
			{
				if ((b & 1) != (a & 1))
					continue;

				int x = (a + b) / 2;
				int y = (a - b) / 2;

				if (x >= 0 && y >= 0 && x < data.width && y < data.height)
				{
					int tile_id = layer->Get(x, y);
					if (tile_id > 0)
					{
						TileSet* tileset = GetTilesetFromTileId(tile_id);

						SDL_Rect r = tileset->GetTileRect(tile_id);
						iPoint pos = MapToWorld(x, y);

						App->render->Blit(tileset->texture, pos.x, pos.y, &r);

						//blits++;
					}
				}

			}
		}
	}
	//LOG("%i", blits);
	//double endTime = timer.ReadMs();
	//LOG("%f", endTime- startTime);
}

int Properties::Get(const char* value, int default_value)
{
	for (std::list<Property*>::iterator it = list.begin(); it != list.end(); it++)
	{
		if (it._Ptr->_Myval->name == value)
			return it._Ptr->_Myval->value;
	}

	return default_value;
}

iPoint j1Map::TileCenterPoint(iPoint tile)
{
	iPoint ret = App->map->MapToWorld(tile.x, tile.y);
	ret.x += data.tile_width / 2;
	ret.y += data.tile_height / 2;
	return ret;

}

iPoint j1Map::GetTilesHalfSize()
{
	return { data.tile_width / 2, data.tile_height / 2 };
}
fPoint j1Map::GetTilesHalfSizeFloat()
{
	return { data.tile_width * 0.5f, data.tile_height * 0.5f};
}

int j1Map::GetMapMaxLenght()
{
	iPoint leftLimit = { 0, data.height };
	iPoint rightLimit = { data.width, 0 };

	//Convert coords to cart space
	leftLimit = MapToWorld(leftLimit.x, leftLimit.y);
	rightLimit = MapToWorld(rightLimit.x, rightLimit.y);

	//Center tile position
	leftLimit = leftLimit + GetTilesHalfSize();
	rightLimit = rightLimit + GetTilesHalfSize();

	return  leftLimit.DistanceManhattan(rightLimit);
}

TileSet* j1Map::GetTilesetFromTileId(int id) 
{

	TileSet* set = nullptr;

	for (std::list<TileSet*>::iterator it = data.tilesets.begin(); it != data.tilesets.end(); it++)
	{
		if (id < it._Ptr->_Myval->firstgid)
		{
			set = it._Ptr->_Prev->_Myval;
			break;
		}
		set = it._Ptr->_Myval;
	}

	return set;
}

SDL_Rect j1Map::GetMapRect() 
{
	return {-((data.width/2) * data.tile_width) + (data.tile_width/2), 0, data.width * data.tile_width, data.height*data.tile_height  };
}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

fPoint j1Map::MapToWorld(float x, float y) const
{
	fPoint ret;

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{
		ret.x = (x - y) * (data.tile_width * 0.5f);
		ret.y = (x + y) * (data.tile_height * 0.5f);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0, 0);

	if (data.type == MAPTYPE_ORTHOGONAL)
	{
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	else if (data.type == MAPTYPE_ISOMETRIC)
	{

		float half_width = data.tile_width * 0.5f;
		float half_height = data.tile_height * 0.5f;
		ret.x = int((x / half_width + y / half_height) / 2) - 1;
		ret.y = int((y / half_height - (x / half_width)) / 2);
	}
	else
	{
		LOG("Unknown map type");
		ret.x = x; ret.y = y;
	}

	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	int relative_id = id - firstgid;
	SDL_Rect rect;
	rect.w = tile_width;
	rect.h = tile_height;
	rect.x = margin + ((rect.w + spacing) * (relative_id % num_tiles_width));
	rect.y = margin + ((rect.h + spacing) * (relative_id / num_tiles_width));
	return rect;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	for (std::list<TileSet*>::iterator it = data.tilesets.begin(); it != data.tilesets.end(); it++)
	{
		App->tex->UnLoad(it._Ptr->_Myval->texture);
		RELEASE(it._Ptr->_Myval);
	}
	data.tilesets.clear();

	// Remove all layers
	for (std::list<MapLayer*>::iterator it = data.downLayers.begin(); it != data.downLayers.end(); it++)
	{
		RELEASE(it._Ptr->_Myval);
	}
	data.downLayers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	std::string tmp;
	tmp.append(folder.c_str());
	tmp.append(file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.c_str());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if (ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for (tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if (ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if (ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.push_back(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for (layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
			MapLayer* lay = new MapLayer();

			ret = LoadLayer(layer, lay);

			if (ret == true) 
			{
				if (lay->properties.Get("afterLayer", 1) == 0) 
				{
					data.topLayers.push_back(lay);
				}
				else
				{
					data.downLayers.push_back(lay);
				}
			}

	}

	if (ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);

		for (std::list<TileSet*>::iterator it = data.tilesets.begin(); it != data.tilesets.end(); it++)
		{
			TileSet* s = it._Ptr->_Myval;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.c_str(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
		}

		for (std::list<MapLayer*>::iterator it = data.downLayers.begin(); it != data.downLayers.end(); it++)
		{
			MapLayer* l = it._Ptr->_Myval;
			LOG("Layer ----");
			LOG("name: %s", l->name.c_str());
			LOG("tile width: %d tile height: %d", l->width, l->height);
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		std::string bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.size() > 0)
		{
			std::string red, green, blue;


			red = bg_color.substr(1, 2);
			green = bg_color.substr(3, 2);
			blue = bg_color.substr(5, 2);

			int v = 0;

			sscanf_s(red.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.c_str(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;

			data.background_color.a = 255;
		}

		std::string orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	set->name.append(tileset_node.attribute("name").as_string());
	set->firstgid = tileset_node.attribute("firstgid").as_int();
	set->tile_width = tileset_node.attribute("tilewidth").as_int();
	set->tile_height = tileset_node.attribute("tileheight").as_int();
	set->margin = tileset_node.attribute("margin").as_int();
	set->spacing = tileset_node.attribute("spacing").as_int();
	pugi::xml_node offset = tileset_node.child("tileoffset");

	if (offset != NULL)
	{
		set->offset_x = offset.attribute("x").as_int();
		set->offset_y = offset.attribute("y").as_int();
	}
	else
	{
		set->offset_x = 0;
		set->offset_y = 0;
	}

	return ret;
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	pugi::xml_node image = tileset_node.child("image");

	if (image == NULL)
	{
		LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
		ret = false;
	}
	else
	{
		set->texture = App->tex->Load(PATH(folder.c_str(), image.attribute("source").as_string()));
		int w, h;
		SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
		set->tex_width = image.attribute("width").as_int();

		if (set->tex_width <= 0)
		{
			set->tex_width = w;
		}

		set->tex_height = image.attribute("height").as_int();

		if (set->tex_height <= 0)
		{
			set->tex_height = h;
		}

		set->num_tiles_width = set->tex_width / set->tile_width;
		set->num_tiles_height = set->tex_height / set->tile_height;
	}

	return ret;
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();
	LoadProperties(node, layer->properties);
	pugi::xml_node layer_data = node.child("data");

	if (layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for (pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

// Load a group of properties from a node and fill a list with it
bool j1Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
	bool ret = false;

	pugi::xml_node data = node.child("properties");

	if (data != NULL)
	{
		pugi::xml_node prop;

		for (prop = data.child("property"); prop; prop = prop.next_sibling("property"))
		{
			Properties::Property* p = new Properties::Property();

			p->name = prop.attribute("name").as_string();
			p->value = prop.attribute("value").as_int();

			properties.list.push_back(p);
		}
	}

	return ret;
}

bool j1Map::CreateWalkabilityMap(int& width, int& height, uchar** buffer)
{
	bool ret = false;

	for (std::list<MapLayer*>::iterator it = data.downLayers.begin(); it != data.downLayers.end(); it++)
	{
		MapLayer* layer = it._Ptr->_Myval;

		if (layer->properties.Get("Navigation", 0) == 0)
			continue;

		uchar* map = new uchar[layer->width*layer->height];
		memset(map, 1, layer->width*layer->height);

		for (int y = 0; y < data.height; ++y)
		{
			for (int x = 0; x < data.width; ++x)
			{
				int i = (y*layer->width) + x;

				int tile_id = layer->Get(x, y);
				TileSet* tileset = (tile_id > 0) ? GetTilesetFromTileId(tile_id) : NULL;

				if (tileset != NULL)
				{
					map[i] = (tile_id - tileset->firstgid) > 0 ? 0 : 1;
					/*TileType* ts = tileset->GetTileType(tile_id);
					if(ts != NULL)
					{
						map[i] = ts->properties.Get("walkable", 1);
					}*/
				}
			}
		}

		*buffer = map;
		width = data.width;
		height = data.height;
		ret = true;

		break;
	}

	return ret;
}

iPoint j1Map::GetMousePositionOnMap()
{
	int x, y;
	App->input->GetMousePosition(x, y);
	y += data.tile_height / 2;
	iPoint p = App->render->ScreenToWorld(x, y);
	p = WorldToMap(p.x, p.y);

	return p;
}