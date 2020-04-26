#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "Console.h"
#include "j1Gui.h"
#include "j1Input.h"
#include "SDL/include/SDL_keyboard.h"
#include "j1Window.h"

Console::Console() {
	name.append("console");
	console_active = false;
}


bool Console::Awake(pugi::xml_node& node) {
	uint w, h;
	App->win->GetWindowSize(w, h);
	background_pos = { node.child("background").attribute("position_x").as_int(),node.child("background").attribute("position_y").as_int(), (int)w, node.child("background").attribute("position_h").as_int() };
	background_red = node.child("background").attribute("red").as_int();
	background_green = node.child("background").attribute("green").as_int();
	background_blue = node.child("background").attribute("blue").as_int();
	background_alpha = node.child("background").attribute("alpha").as_int();

	input_pos = { node.child("input").attribute("position_x").as_int(),node.child("input").attribute("position_y").as_int(), (int)w, node.child("input").attribute("position_h").as_int() };
	input_red = node.child("input").attribute("red").as_int();
	input_green = node.child("input").attribute("green").as_int();
	input_blue = node.child("input").attribute("blue").as_int();
	input_alpha = node.child("input").attribute("alpha").as_int();

	output_pos = { node.child("output").attribute("position_x").as_int(),node.child("output").attribute("position_y").as_int(),node.child("output").attribute("position_w").as_int(),
	node.child("output").attribute("position_h").as_int() };
	output_drag_area = { node.child("output").attribute("d_area_x").as_int(),node.child("output").attribute("d_area_y").as_int(),node.child("output").attribute("d_area_w").as_int(),
	node.child("output").attribute("d_area_h").as_int() };

	label = "";
	active = true;

	return true;
}

bool Console::PreUpdate() {
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) == KEY_DOWN)
	{
		console_active = !console_active;
		if (console_active == true) {
			ActivateConsole();
		}
		else {
			CloseConsole();
		}
	}
	if (console_active == true) {
		console_input->focus = true;
		if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
			std::string argument = CheckCommand();
			ExecuteCommand(argument);
			console_input->SetLabel("");
			console_input->SetPositionToZero();
		}
		label = console_input->GetLabel();

		int i = 0;
		for (std::list<std::string>::iterator it = App->logs.begin(); it != App->logs.end(); it++)
		{
			console_log->SetListOfStrings(it._Ptr->_Myval.c_str(), i);
			i++;
		}
	}
	return true;
}

void Console::ActivateConsole() 
{
	console_background = (ImageUI*)App->gui->CreateUIElement(Type::IMAGE, nullptr, background_pos, "", background_red, background_green, background_blue, background_alpha);
	console_background->SetPriority(2);
	console_log = (ListTextsUI*)App->gui->CreateUIElement(Type::LISTTEXTS, console_background, output_pos, { 0,0,0,0 }, App->logs.begin()->c_str(), { 0,0,0,0 }, { 0,0,0,0 }, true,
		output_drag_area, nullptr, 0, true);
	console_log->SetPriority(2);
	console_input = (TextInputUI*)App->gui->CreateUIElement(Type::INPUT, nullptr, input_pos, "", input_red, input_green, input_blue, input_alpha);
	console_input->SetPriority(2);
	console_input->SetLabel(label.c_str());
}

void Console::CloseConsole() 
{
	App->gui->DeleteUIElement(console_log);
	App->gui->DeleteUIElement(console_background);
	App->gui->DeleteUIElement(console_input);
}

std::string Console::CheckCommand() {
	std::string argument = "";
	std::string command_text = console_input->GetLabel();
	if (!strcmp(command_text.c_str(), "list"))
		command = commands::list;
	else if (!strcmp(command_text.c_str(), "god_mode") || !strcmp(command_text.c_str(), "god mode") || !strcmp(command_text.c_str(), "godmode"))
		command = commands::god_mode;
	else if (!strcmp(command_text.c_str(), "quit") || !strcmp(command_text.c_str(), "exit"))
		command = commands::quit;
	else if (!strcmp(command_text.c_str(), "FPS") || !strcmp(command_text.c_str(), "Fps") || !strcmp(command_text.c_str(), "fps"))
		command = commands::FPS;
	else if (!strcmp(command_text.c_str(), "map"))
		command = commands::map;
	else if (!strcmp(command_text.c_str(), "fullscreen") || !strcmp(command_text.c_str(), "FULLSCREEN"))
		command = commands::fullscreen;
	else {
		std::string three_letters_command = argument = console_input->GetLabel();
		int num_of_letters = three_letters_command.size();
		for (int i = 3; i < num_of_letters; i++) {
			three_letters_command = three_letters_command.erase(3);
		}
		for (int i = 0; i < 4; i++) {
			argument = argument.erase(0);
		}
		if (!strcmp(three_letters_command.c_str(), "FPS") || !strcmp(three_letters_command.c_str(), "Fps") || !strcmp(three_letters_command.c_str(), "fps"))
			command = commands::FPS;
		else if (!strcmp(three_letters_command.c_str(), "map"))
			command = commands::map;
		else
			command = commands::none;
	}
	return argument;
}

void Console::ExecuteCommand(std::string argument) {
	int fps = 0;
	switch (command) {
	case commands::list:
		LOG("Commands available: list, god_mode, quit, FPS, map");
		break;

	case commands::god_mode:
		LOG("GodMode");
		break;

	case commands::quit:
		LOG("Exiting game");
		exitGame = true;
		break;

	case commands::fullscreen:
		if (App->win->ToggleFullscreen())
		{
			LOG("Fullscreen mode is : OFF");
		}
		else
		{
			LOG("Fullscreen mode is : ON");
		}
		break;

	case commands::FPS:
		LOG("FPS");
		break;

	case commands::map:
		LOG("Map");
		break;

	case commands::none:
		LOG("Command not found");
		break;
	}
}