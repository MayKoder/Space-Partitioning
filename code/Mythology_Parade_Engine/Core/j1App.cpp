#include <iostream> 
#include <sstream> 

#include "p2Defs.h"
#include "p2Log.h"

#include "j1Window.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Fonts.h"
#include "j1Gui.h"
#include "Console.h"
#include "EntityManager.h"
#include "j1App.h"


// Constructor
j1App::j1App(int argc, char* args[]) : argc(argc), args(args)
{
	PERF_START(ptimer);

	input = new j1Input();
	win = new j1Window();
	render = new j1Render();
	tex = new j1Textures();
	scene = new j1Scene();
	map = new j1Map();
	font = new j1Fonts();
	gui = new j1Gui();
	console = new Console();
	entityManager = new EntityManager();

	// Ordered for awake / Start / Update
	// Reverse order of CleanUp
	AddModule(input);
	AddModule(win);
	AddModule(tex);
	AddModule(map);
	AddModule(font);


	// scene last
	AddModule(scene);

	AddModule(gui);
	AddModule(console);

	// entities
	AddModule(entityManager);

	// render last to swap buffer
	AddModule(render);


	PERF_PEEK(ptimer);
}

// Destructor
j1App::~j1App()
{
	// release modules
	//INFO: How to itinerate all elements of a STD list
	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
	{
		RELEASE(it._Ptr->_Myval);
	}
	modules.clear();
}

void j1App::AddModule(j1Module* module)
{
	module->Init();
	modules.push_back(module);
}

// Called before render is available
bool j1App::Awake()
{
	PERF_START(ptimer);

	pugi::xml_document	config_file;
	pugi::xml_node		config;
	pugi::xml_node		app_config;

	bool ret = false;
		
	config = LoadConfig(config_file);

	if(config.empty() == false)
	{
		// self-config
		ret = true;
		app_config = config.child("app");
		title.append(app_config.child("title").child_value());
		organization.append(app_config.child("organization").child_value());

		int cap = app_config.attribute("framerate_cap").as_int(-1);

		if(cap > 0)
		{
			capped_ms = 1000 / cap;
		}
	}

	if(ret == true)
	{
		for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		{
			ret = it._Ptr->_Myval->Awake(config.child(it._Ptr->_Myval->name.c_str()));
		}
	}

	PERF_PEEK(ptimer);

	return ret;
}

// Called before the first frame
bool j1App::Start()
{
	PERF_START(ptimer);
	bool ret = true;


	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
	{
		if(it._Ptr->_Myval->active==true)
			ret = it._Ptr->_Myval->Start();
	}

	startup_time.Start();

	PERF_PEEK(ptimer);

	return ret;
}

// Called each loop iteration
bool j1App::Update()
{
	bool ret = true;
	PrepareUpdate();

	if(input->GetWindowEvent(WE_QUIT) == true)
		ret = false;

	if(ret == true)
		ret = PreUpdate();

	if(ret == true)
		ret = DoUpdate();

	if(ret == true)
		ret = PostUpdate();

	FinishUpdate();
	return ret;
}

// ---------------------------------------------
pugi::xml_node j1App::LoadConfig(pugi::xml_document& config_file) const
{
	pugi::xml_node ret;

	pugi::xml_parse_result result = config_file.load_file("config.xml");

	if(result == NULL)
		LOG("Could not load map xml file config.xml. pugi error: %s", result.description());
	else
		ret = config_file.child("config");

	return ret;
}

// ---------------------------------------------
void j1App::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = frame_time.ReadSec();
	frame_time.Start();
}

// ---------------------------------------------
void j1App::FinishUpdate()
{
	if(want_to_save == true)
		SavegameNow();

	if(want_to_load == true)
		LoadGameNow();

	// Framerate calculations --

	if(last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	static char title[256];
	sprintf_s(title, 256, "Av.FPS: %.2f Last Frame Ms: %.2u Last sec frames: %i Last dt: %.3f Time since startup: %.3f Frame Count: %lu ",
			  avg_fps, last_frame_ms, frames_on_last_update, dt, seconds_since_startup, frame_count);
	App->win->SetTitle(title);

	if(capped_ms > 0 && (int)last_frame_ms < capped_ms)
	{
		j1PerfTimer t;
		SDL_Delay(capped_ms - last_frame_ms);
	}
}

// Call modules before each loop iteration
bool j1App::PreUpdate()
{
	bool ret = true;

	if (restart_scene == true) {
		restart_scene = false;
		RestartScene();
	}

	j1Module* pModule = NULL;

	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
	{

		pModule = it._Ptr->_Myval;
		if (pModule->active == false) 
		{
			continue;
		}

		ret = it._Ptr->_Myval->PreUpdate();
	}

	return ret;
}

// Call modules on each loop iteration
bool j1App::DoUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
	{
		pModule = it._Ptr->_Myval;
		if (pModule->active == false)
		{
			continue;
		}

		ret = it._Ptr->_Myval->Update(dt);
	}

	return ret;
}

// Call modules after each loop iteration
bool j1App::PostUpdate()
{
	bool ret = true;
	j1Module* pModule = NULL;

	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
	{
		pModule = it._Ptr->_Myval;
		if (pModule->active == false)
		{
			continue;
		}

		ret = it._Ptr->_Myval->PostUpdate();
	}

	return ret;
}

// Called before quitting
bool j1App::CleanUp()
{
	PERF_START(ptimer);
	bool ret = true;

	modules.reverse();
	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
	{
		if(it._Ptr->_Myval != nullptr)
			ret = it._Ptr->_Myval->CleanUp();
	}

	PERF_PEEK(ptimer);
	return ret;
}

// ---------------------------------------
int j1App::GetArgc() const
{
	return argc;
}

// ---------------------------------------
const char* j1App::GetArgv(int index) const
{
	if(index < argc)
		return args[index];
	else
		return NULL;
}

// ---------------------------------------
const char* j1App::GetTitle() const
{
	return title.c_str();
}

// ---------------------------------------
float j1App::GetDT() const
{
	return dt;
}

// ---------------------------------------
const char* j1App::GetOrganization() const
{
	return organization.c_str();
}

// Load / Save
void j1App::LoadGame(const char* file)
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list
	want_to_load = true;
	//load_game.create("%s%s", fs->GetSaveDirectory(), file);
}

// ---------------------------------------
void j1App::SaveGame(const char* file) const
{
	// we should be checking if that file actually exist
	// from the "GetSaveGames" list ... should we overwrite ?

	want_to_save = true;
	save_game.append(file);
}

// ---------------------------------------
void j1App::GetSaveGames(std::list<std::string>& list_to_fill) const
{
	// need to add functionality to file_system module for this to work
}

bool j1App::LoadGameNow()
{
	bool ret = false;

	pugi::xml_document data;
	pugi::xml_node root;

	pugi::xml_parse_result result = data.load_file(load_game.c_str());

	if(result != NULL)
	{
		LOG("Loading new Game State from %s...", load_game.c_str());

		root = data.child("game_state");

		ret = true;
		j1Module* item = NULL;


		for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
		{
			ret = it._Ptr->_Myval->Load(root.child(it._Ptr->_Myval->name.c_str()));
			item = it._Ptr->_Myval;
		}


		data.reset();
		if(ret == true)
			LOG("...finished loading");
		else
			LOG("...loading process interrupted with error on module %s", (item != NULL) ? item->name.c_str() : "unknown");
	}
	else
		LOG("Could not parse game state xml file %s. pugi error: %s", load_game.c_str(), result.description());

	want_to_load = false;
	return ret;
}

bool j1App::SavegameNow()
{
	bool ret = true;

	LOG("Saving Game State to %s...", save_game.c_str());

	// xml object were we will store all data
	pugi::xml_document data;
	pugi::xml_node root;
	
	root = data.append_child("game_state");

	j1Module* item = NULL;

	for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end() && ret == true; it++)
	{
		ret = it._Ptr->_Myval->Save(root.append_child(it._Ptr->_Myval->name.c_str()));
		item = it._Ptr->_Myval;
	}

	if(ret == true)
	{
		std::stringstream stream;
		data.save(stream);

		// we are done, so write data to disk
		//fs->Save(save_game.GetString(), stream.str().c_str(), stream.str().length());
		LOG("... finished saving", save_game.c_str());
	}
	else
		LOG("Save process halted from an error in module %s", (item != NULL) ? item->name.c_str() : "unknown");

	data.reset();
	want_to_save = false;
	return ret;
}

bool j1App::RestartScene() {
	bool ret = true;

	for (int i = 0; i < 2; i++) {
		for (std::list<j1Module*>::iterator it = modules.begin(); it != modules.end(); it++)
		{
			if (it._Ptr->_Myval->destroy == true) {
				if(i==0)
					it._Ptr->_Myval->CleanUp();
				else {
					it._Ptr->_Myval->Start();
					it._Ptr->_Myval->destroy = false;
				}
			}
		}
	}
	return ret;
}