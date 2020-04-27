#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Fonts.h"
#include "j1Input.h"
#include "j1Window.h"
#include "j1Gui.h"

j1Gui::j1Gui() : j1Module()
{
	name.append("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name_num_0 = conf.child("atlas_0").attribute("file").as_string("");
	atlas_file_name_num_1 = conf.child("atlas_1").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas_num_0 = App->tex->Load(atlas_file_name_num_0.c_str());
	atlas_num_1 = App->tex->Load(atlas_file_name_num_1.c_str());
	for (int i = 0; i < 9; i++) {
		sfx_UI[i] = 0;
	}
	cursor_tex = App->tex->Load("gui/cursors.png");
	return true;
}

// Update all guis
bool j1Gui::PreUpdate()
{
	bool mouse = false;
	lockClick = false;
	int count = 0;
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN || App->input->GetMouseButtonDown(1) == KEY_REPEAT) 
	{
		for (int i = UIs.size(); i >= 0 && mouse == false; i--) 
		{
			std::list<UI*>::iterator it = UIs.begin();
			std::advance(it, i);

			if (it._Ptr->_Myval != UIs.end()._Ptr->_Myval) 
			{
				mouse = it._Ptr->_Myval->CheckMouse();
			}
			if (mouse == true)count = i;
		}
	}
	if (mouse == true) 
	{
		std::list<UI*>::iterator it = UIs.begin();
		std::advance(it, count);

		if (it._Ptr->_Myval != UIs.end()._Ptr->_Myval)
			it._Ptr->_Myval->Move();
	}
	for(std::list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++)
	{
		it._Ptr->_Myval->PreUpdate();
	}
	return true;
}

// Called after all Updates
bool j1Gui::PostUpdate()
{

		for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++)
		{
			it._Ptr->_Myval->PostUpdate();
		}

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");
	for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++)
	{
		it._Ptr->_Myval->CleanUp();
		delete it._Ptr->_Myval;
	}
	UIs.clear();

	if (atlas_num_0)
	{
		App->tex->UnLoad(atlas_num_0);
	}
	if (atlas_num_1)
	{
		App->tex->UnLoad(atlas_num_1);
	}
	App->tex->UnLoad(cursor_tex);
	return true;
}

// const getter for atlas
const SDL_Texture* j1Gui::GetAtlas(int number_atlas) const
{
	if (number_atlas == 0)
		return atlas_num_0;
	else if (number_atlas == 1)
		return atlas_num_1;
}

// class Gui ---------------------------------------------------

UI* j1Gui::CreateUIElement(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, std::string str, SDL_Rect sprite2, SDL_Rect sprite3, bool drageable, SDL_Rect drag_area, j1Module* s_listener, int audio,
	bool console, float drag_position_scroll_bar, int number_atlas)
{
	UI* ui = nullptr;
	SDL_Color colour;
	bool title;
	switch (type)
	{
	case Type::BUTTON:
		ui = new ButtonUI(Type::BUTTON, p, r, sprite, sprite2, sprite3, true, true, drag_area, audio);
		break;
	case Type::IMAGE:
		ui = new ImageUI(Type::IMAGE, p, r, sprite, drageable, drageable, drag_area, drag_position_scroll_bar);
		break;
	case Type::WINDOW:
		ui = new WindowUI(Type::WINDOW, p, r, sprite, drageable, drageable, drag_area);
		break;
	case Type::TEXT:
		colour = { (Uint8)sprite2.x,(Uint8)sprite2.y,(Uint8)sprite2.w,(Uint8)sprite2.h };
		if (sprite3.x == 0)
			title = false;
		else
			title = true;
		ui = new TextUI(Type::TEXT, p, r, str, drageable, drageable, drag_area, console, colour, title);
		break;
	case Type::LISTTEXTS:
		ui = new ListTextsUI(Type::LISTTEXTS, p, r, str, drageable, drageable, drag_area, console);
		break;
	}

	ui->active = true;
	ui->name = str.c_str();
	ui->num_atlas = number_atlas;

	if (s_listener)
	{
		ui->listener = s_listener;
	}
	else
	{
		ui->listener = nullptr;
	}


	//UIs.add(ui);

	UIs.push_back(ui);
	return ui;
}

UI* j1Gui::CreateUIElement(Type type, UI* p, SDL_Rect r, std::string str, int re, int g, int b, int a, bool drageable, SDL_Rect drag_area, j1Module* s_listener)
{
	UI* ui = nullptr;
	switch (type)
	{
	case Type::IMAGE:
		ui = new ImageUI(Type::IMAGE, p, r, re, g, b, a, drageable, drageable, drag_area);
		break;
	case Type::INPUT:
		ui = new TextInputUI(Type::INPUT, p, r, re, g, b, a, "", drageable, true, drag_area);
		break;
	}

	ui->active = true;
	ui->name = str.c_str();

	if (s_listener)
	{
		ui->listener = s_listener;
	}
	else
	{
		ui->listener = nullptr;
	}


	//UIs.add(ui);

	UIs.push_back(ui);
	return ui;
}



bool j1Gui::DeleteUIElement(UI* ui) 
{

	std::list<UI*>::iterator it = UIs.begin();
	it = std::find(UIs.begin(), UIs.end(), ui);

	if (it == UIs.end())return false;
	else
	{
		it._Ptr->_Myval->CleanUp();
		UIs.remove(ui);
		delete it._Ptr->_Myval;
		return true;
	}
}

void j1Gui::ChangeDebug() 
{
	for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++)
	{
		it._Ptr->_Myval->debug = !it._Ptr->_Myval->debug;
	}
}

void j1Gui::ChangeFocus() 
{
	bool exit = false;
	bool focus = false;
	int count = 0;
	for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end() && exit == false; it++)
	{

		bool focusable = it._Ptr->_Myval->CheckFocusable();
		if (focusable == true) {
			count++;
			if (focus == true) {
				it._Ptr->_Myval->focus = true;
				exit = true;
			}
			else {
				focus = it._Ptr->_Myval->focus;
				it._Ptr->_Myval->focus = false;
			}
		}
	}
	if (count > 0 && exit == false) 
	{
		for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end() && exit == false; it++)
		{
			bool focusable = it._Ptr->_Myval->CheckFocusable();
			if (focusable == true) {
				it._Ptr->_Myval->focus = true;
				exit = true;
			}
		}
	}
}

void j1Gui::DeleteFocus() {
	for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end(); it++)
	{
		it._Ptr->_Myval->focus = false;
	}
}

void j1Gui::ClearUI()
{
	UIs.clear();
}

void j1Gui::WorkWithTextInput(std::string text) {
	bool exit = false;
	for (std::list<UI*>::iterator it = UIs.begin(); it != UIs.end() && exit == false; it++)
	{
		if (it._Ptr->_Myval->type == Type::INPUT &&it._Ptr->_Myval->focus == true)
		{
			TextInputUI* text_ui = (TextInputUI*)it._Ptr->_Myval;
			text_ui->ChangeLabel(text);
		}
	}
}

UI::UI(Type s_type, SDL_Rect r, UI* p, bool d, bool f, SDL_Rect d_area, bool consol)
{
	name.append("UI");
	type = s_type;
	drageable = d;
	focusable = f;
	screen_rect = { r.x,r.y,r.w,r.h };
	parent = p;
	if (parent != nullptr) {
		local_rect = { screen_rect.x - parent->screen_rect.x, screen_rect.y - parent->screen_rect.y, screen_rect.w, screen_rect.h };
	}
	else {
		local_rect = screen_rect;
	}
	mask_rect = screen_rect;
	debug = false;
	focus = false;
	drag_area = d_area;
	console = consol;
	priority = 1;
	num_atlas = 0;
}

bool UI::PreUpdate() {
	UI* ui = this;

	screen_rect.x = local_rect.x;
	screen_rect.y = local_rect.y;
	while (ui->parent != nullptr) {
		screen_rect.x += ui->parent->local_rect.x;
		screen_rect.y += ui->parent->local_rect.y;
		ui = ui->parent;
	}

	uint win_x, win_y;
	App->win->GetWindowSize(win_x, win_y);
	mask_rect = screen_rect;

	if (parent != nullptr) {
		if (mask_rect.x < parent->mask_rect.x) {
			mask_rect.x += parent->mask_rect.x - mask_rect.x;
			mask_rect.w -= parent->mask_rect.x - mask_rect.x;
		}
		else if (mask_rect.x + mask_rect.w > parent->mask_rect.x + parent->mask_rect.w) {
			mask_rect.w -= mask_rect.x + mask_rect.w - parent->mask_rect.x - parent->mask_rect.w;
		}
		if (mask_rect.y < parent->mask_rect.y) {
			mask_rect.y += parent->mask_rect.y - mask_rect.y;
			mask_rect.h -= parent->mask_rect.y - mask_rect.y;
		}
		else if (mask_rect.y + mask_rect.h > parent->mask_rect.y + parent->mask_rect.h) {
			mask_rect.h -= mask_rect.y + mask_rect.h - parent->mask_rect.y - parent->mask_rect.h;
		}
	}
	else {
		if (mask_rect.x < 0) {
			mask_rect.w -= mask_rect.x;
			mask_rect.x = 0;
		}
		else if (mask_rect.x + mask_rect.w > win_x) {
			mask_rect.w -= mask_rect.x + mask_rect.w - win_x;
		}
		if (mask_rect.y < 0) {
			mask_rect.h -= mask_rect.y;
			mask_rect.y = 0;
		}
		else if (mask_rect.y + mask_rect.h > win_y) {
			mask_rect.h -= mask_rect.y + mask_rect.h - win_y;
		}
	}
	return true;
}

bool UI::PostUpdate() {
	if (debug == true) {
		App->render->DrawQuad(screen_rect, 255, 0, 0, 255, false, false);
	}
	return true;
}

SDL_Rect UI::GetScreenRect()
{
	return screen_rect;
}
SDL_Rect UI::GetParentScreenRect()
{
	if (parent != nullptr)
		return parent->screen_rect;
	else {
		uint width, height;
		App->win->GetWindowSize(width, height);
		return { 0,0,(int)width,(int)height };
	}
}
SDL_Rect UI::GetLocalRect() {
	return local_rect;
}
iPoint UI::GetScreenPos() {
	return { screen_rect.x,screen_rect.y };
}
iPoint UI::GetScreenToWorldPos() {
	uint scale = App->win->GetScale();
	return { screen_rect.x / (int)scale,screen_rect.y / (int)scale };
}
iPoint UI::GetLocalPos() {
	return { local_rect.x,local_rect.y };
}
void UI::SetLocalPos(iPoint pos) {
	iPoint r = { -local_rect.x + pos.x,-local_rect.y + pos.y };
	local_rect.x = pos.x;
	local_rect.y = pos.y;
	screen_rect.x += r.x;
	screen_rect.y += r.y;
}

void UI::SetRect(SDL_Rect rect) {
	screen_rect = rect;
	if (parent != nullptr) {
		local_rect = { screen_rect.x - parent->screen_rect.x, screen_rect.y - parent->screen_rect.y, screen_rect.w, screen_rect.h };
	}
	else {
		local_rect = screen_rect;
	}
	mask_rect = screen_rect;
}

void UI::SetScreenRect(SDL_Rect rect) {
	screen_rect = rect;
}

void UI::UpdateLocalRect() {
	if (parent != nullptr) {
		local_rect = { screen_rect.x - parent->screen_rect.x, screen_rect.y - parent->screen_rect.y, screen_rect.w, screen_rect.h };
	}
	else {
		local_rect = screen_rect;
	}
}

bool UI::CheckMouse() 
{
	if (drageable == true) 
	{
		int x, y;
		App->input->GetMousePosition(x, y);
		if ((x >= screen_rect.x && x <= screen_rect.x + screen_rect.w && y >= screen_rect.y && y <= screen_rect.y + screen_rect.h) || focus == true)
			return true;
	}
	return false;
}

bool UI::Move() {
	int x, y;
	App->input->GetMouseMotion(x, y);
	x *= App->win->GetScale();
	y *= App->win->GetScale();
	if (screen_rect.x + x >= drag_area.x && screen_rect.x + screen_rect.w + x <= drag_area.x + drag_area.w) {
		local_rect.x += x;
		quad.x += x;
	}
	else if (screen_rect.y + y >= drag_area.y && screen_rect.y + screen_rect.h + y <= drag_area.y + drag_area.h) {
		local_rect.y += y;
		quad.y += y;
	}
	return true;
}

void UI::SetPriority(int prior) {
	priority = prior;
}

SDL_Rect UI::Check_Printable_Rect(SDL_Rect sprite, iPoint& dif_sprite) {
	if (mask_rect.x > screen_rect.x) {
		dif_sprite.x = mask_rect.x - screen_rect.x;
		sprite.x += dif_sprite.x;
		sprite.w -= dif_sprite.x;
	}
	else if (mask_rect.w < screen_rect.w) {
		sprite.w -= screen_rect.w - mask_rect.w;
	}
	if (mask_rect.y > screen_rect.y) {
		dif_sprite.y = mask_rect.y - screen_rect.y;
		sprite.y += dif_sprite.y;
		sprite.h -= dif_sprite.y;
	}
	else if (mask_rect.h < screen_rect.h) {
		sprite.h -= screen_rect.h - mask_rect.h;
	}
	return sprite;
}

SDL_Rect UI::Check_Printable_Rect(SDL_Rect sprite, iPoint& dif_sprite, SDL_Rect quad) {
	SDL_Rect parent_screen_rect = GetParentScreenRect();
	if (mask_rect.x > quad.x) {
		dif_sprite.x = mask_rect.x - quad.x;
		sprite.x += dif_sprite.x;
		sprite.w -= dif_sprite.x;
	}
	else if (mask_rect.w < quad.w) {
		sprite.w -= quad.w - mask_rect.w;
	}
	if (parent_screen_rect.y > quad.y) {
		dif_sprite.y = parent_screen_rect.y - quad.y;
		sprite.y += dif_sprite.y;
		sprite.h -= dif_sprite.y;
	}
	else if (parent_screen_rect.y + parent_screen_rect.h < quad.y + quad.h) {
		sprite.h -= quad.y + quad.h - parent_screen_rect.y - parent_screen_rect.h;
	}
	return sprite;
}

ImageUI::ImageUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, bool d, bool f, SDL_Rect d_area, float drag_position_scroll_bar) :UI(type, r, p, d, f, d_area) {
	name.append("ImageUI");
	sprite1 = sprite;
	quad = r;
	SDL_Rect drag_area = GetDragArea();
	drag_position_0 = { drag_area.x, drag_area.y };
	drag_position_1 = { drag_area.w + drag_area.x - GetLocalRect().w,drag_area.h + drag_area.y - GetLocalRect().h };
	square = false;
	red = green = blue = alpha = 0;
	if (drag_position_scroll_bar != -1) {
		quad.x = drag_position_0.x + (drag_position_scroll_bar * (drag_position_1.x - drag_position_0.x));
		SetScreenRect(quad);
		UpdateLocalRect();
	}
}

ImageUI::ImageUI(Type type, UI* p, SDL_Rect r, int re, int g, int b, int a, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area, true) {
	name.append("ImageUI");
	sprite1 = { 0,0,0,0 };
	quad = r;
	SDL_Rect drag_area = GetDragArea();
	drag_position_0 = { drag_area.x, drag_area.y };
	drag_position_1 = { drag_area.w + drag_area.x - GetLocalRect().w,drag_area.h + drag_area.y - GetLocalRect().h };
	square = true;
	red = re;
	green = g;
	blue = b;
	alpha = a;
}

bool ImageUI::PreUpdate() {
	int x, y;
	iPoint initial_position = GetScreenPos();
	App->input->GetMousePosition(x, y);
	if (CheckFocusable() == true && (x >= GetScreenPos().x && x <= GetScreenPos().x + GetScreenRect().w && y >= GetScreenPos().y && y <= GetScreenPos().y + GetScreenRect().h)) {
		if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
			App->gui->DeleteFocus();
			focus = true;
		}
	}
	if (focus == true && App->input->GetMouseButtonDown(1) == KEY_UP) {
		focus = false;
	}
	UI::PreUpdate();
	if (initial_position != GetScreenPos()) {
		fPoint drag_position = GetDragPositionNormalized();
		if (listener)
		{
			listener->OnClick(this, drag_position.x);
		}
	}
	return true;
}

bool ImageUI::PostUpdate() {
	iPoint dif_sprite = { 0,0 };
	if (square == false) {
		SDL_Rect sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
		quad.x = GetScreenPos().x + dif_sprite.x;
		quad.y = GetScreenPos().y + dif_sprite.y;
		if (this->active) App->render->BlitInsideQuad((SDL_Texture*)App->gui->GetAtlas(num_atlas), sprite, quad);
	}
	else if (this->active) {
		App->render->DrawQuad(quad, red, green, blue, alpha, true, false);
	}
	UI::PostUpdate();
	return true;
}

fPoint ImageUI::GetDragPositionNormalized() {
	fPoint position_normalized;
	position_normalized.x = GetScreenPos().x - drag_position_0.x;
	position_normalized.y = GetScreenPos().y - drag_position_0.y;
	position_normalized.x /= drag_position_1.x - drag_position_0.x;
	position_normalized.y /= drag_position_1.y - drag_position_0.y;
	return position_normalized;
}

WindowUI::WindowUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area) {
	name.append("WindowUI");
	sprite1 = sprite;
	quad = r;
}

bool WindowUI::PostUpdate() {
	iPoint dif_sprite = { 0,0 };
	SDL_Rect sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
	quad.x = GetScreenPos().x + dif_sprite.x;
	quad.y = GetScreenPos().y + dif_sprite.y;
	if (this->active)App->render->BlitInsideQuad((SDL_Texture*)App->gui->GetAtlas(num_atlas), sprite, quad);

	UI::PostUpdate();
	return true;
}


TextUI::TextUI(Type type, UI* p, SDL_Rect r, std::string str, bool d, bool f, SDL_Rect d_area, bool console, SDL_Color coulor, bool title) :UI(type, r, p, d, f, d_area, console) 
{
	name.append("TextUI");
	stri = str.c_str();
	quad = r;
	color = coulor;
	title_default = title;
}

bool TextUI::PostUpdate() {
	SDL_Rect rect = { 0,0,0,0 };
	iPoint dif_sprite = { 0,0 };

  
	SDL_Texture* text;
	if(title_default==false)
		text = App->font->Print(stri.c_str(), color);
	else
		text = App->font->Print(stri.c_str(), color, App->font->default_title);

	SDL_QueryTexture(text, NULL, NULL, &rect.w, &rect.h);


	SDL_Rect sprite = UI::Check_Printable_Rect(rect, dif_sprite);
	if (this->active && this->GetConsole() == false)App->render->Blit(text, GetScreenToWorldPos().x + dif_sprite.x, GetScreenToWorldPos().y + dif_sprite.y, &sprite, 0.0F);
	else if (this->active) App->render->Blit(text, quad.x + dif_sprite.x, quad.y + dif_sprite.y, &sprite, 0.0F);
	UI::PostUpdate();

	App->tex->UnLoad(text);

	return true;
}

void TextUI::SetString(std::string new_string) 
{
	stri = new_string;
}

ListTextsUI::ListTextsUI(Type type, UI* p, SDL_Rect r, std::string str, bool d, bool f, SDL_Rect d_area, bool console) :UI(type, r, p, d, f, d_area, console) {
	name.append("ListTextsUI");
	stri.push_back(str);
	number_of_stri = stri.size();
	quad = r;
}



bool ListTextsUI::PostUpdate() 
{

	SDL_Rect rect = { 0,0,0,0 };
	iPoint dif_sprite = { 0,0 };

	for (int i = 0; i < number_of_stri; i++) 
	{
		dif_sprite = { 0,0 };

		std::list<std::string>::iterator it = stri.begin();
		std::advance(it, i);

		if (it._Ptr->_Myval != stri.end()._Ptr->_Myval) 
		{
			SDL_Texture* text = App->font->Print(it->c_str(), { 255,255,255,255 });

			SDL_QueryTexture(text, NULL, NULL, &rect.w, &rect.h);


			SDL_Rect sprite = UI::Check_Printable_Rect(rect, dif_sprite, { quad.x,quad.y + (quad.h * i),quad.w,quad.h });
			if (this->active && this->GetConsole() == false)App->render->Blit(text, GetScreenToWorldPos().x + dif_sprite.x, GetScreenToWorldPos().y + dif_sprite.y, &sprite, 0.0F);
			else if (this->active) App->render->Blit(text, quad.x + dif_sprite.x, quad.y + dif_sprite.y + (i * quad.h), &sprite, 0.0F);

			App->tex->UnLoad(text);
		}
	}
	UI::PostUpdate();

	return true;
}

void ListTextsUI::SetListOfStrings(std::string string, int position) 
{
	if (position > number_of_stri) {
		stri.push_back(string);
		number_of_stri++;
		SDL_Rect screen_rect = GetScreenRect();
		SDL_Rect parent_screen_rect = GetParentScreenRect();
		screen_rect.h += quad.h;
		quad.y = screen_rect.y = (parent_screen_rect.y + parent_screen_rect.h) - screen_rect.h;
		SetScreenRect(screen_rect);
		UpdateLocalRect();
	}
}

ButtonUI::ButtonUI(Type type, UI* p, SDL_Rect r, SDL_Rect sprite, SDL_Rect spriten2, SDL_Rect spriten3, bool d, bool f, SDL_Rect d_area, int audio) :UI(type, r, p, d, f, d_area) {
	name.append("ButtonUI");
	sprite1 = sprite;
	sprite2 = spriten2;
	sprite3 = spriten3;
	over = false;
	pushed = false;
	quad = r;
	isLocked = false;
	front = true;
	click_sfx = App->gui->sfx_UI[audio];
}

bool ButtonUI::PostUpdate() {
	SDL_Rect sprite;
	iPoint dif_sprite = { 0,0 };
	if (pushed == true || isLocked) {
		sprite = UI::Check_Printable_Rect(sprite2, dif_sprite);
	}
	else if (over == true) {
		sprite = UI::Check_Printable_Rect(sprite1, dif_sprite);
	}
	else {
		sprite = UI::Check_Printable_Rect(sprite3, dif_sprite);
	}
	//App->render->Blit((SDL_Texture*)App->gui->GetAtlas(), GetScreenToWorldPos().x + dif_sprite.x, GetScreenToWorldPos().y + dif_sprite.y, &sprite, 0.f);

	quad.x = GetScreenPos().x + dif_sprite.x;
	quad.y = GetScreenPos().y + dif_sprite.y;
	if (this->active)App->render->BlitInsideQuad((SDL_Texture*)App->gui->GetAtlas(num_atlas), sprite, quad);

	UI::PostUpdate();
	return true;
}

bool ButtonUI::PreUpdate() {
	int x, y;
	App->input->GetMousePosition(x, y);
	bool pushing = false;
	if (front == true && ((x >= GetScreenPos().x && x <= GetScreenPos().x + GetScreenRect().w && y >= GetScreenPos().y && y <= GetScreenPos().y + GetScreenRect().h) || focus == true))
		over = true;
	else over = false;
	bool button = false;
	if (App->input->GetMouseButtonDown(1) == KEY_DOWN || App->input->GetMouseButtonDown(1) == KEY_REPEAT) {
		pushing = true;
	}
	if (App->input->GetMouseButtonDown(1) == KEY_UP || App->input->GetKey(SDL_SCANCODE_RETURN))
		button = true;
	if (over == true && button == true)
		pushed = true;
	else pushed = false;
	if (pushed && !App->gui->lockClick && !isLocked)
	{
		//Button clicked
		if (listener)
		{
			listener->OnClick(this);
		}
		App->gui->lockClick = true;
	}
	if (pushing == true && over == true)
		pushed = true;
	UI::PreUpdate();

	return true;
}


TextInputUI::TextInputUI(Type type, UI* p, SDL_Rect r, int re, int g, int b, int a, std::string str, bool d, bool f, SDL_Rect d_area) :UI(type, r, p, d, f, d_area, true) {
	name.append("TextInputUI");
	sprite1 = { 0,0,0,0 };
	quad = r;
	label = str;
	text_input = false;
	position = 0;
	square = true;
	red = re;
	green = g;
	blue = b;
	alpha = a;
}

bool TextInputUI::PreUpdate() {
	int x, y;
	App->input->GetMousePosition(x, y);
	if ((x >= GetScreenPos().x && x <= GetScreenPos().x + GetScreenRect().w && y >= GetScreenPos().y && y <= GetScreenPos().y + GetScreenRect().h)) {
		if (App->input->GetMouseButtonDown(1) == KEY_DOWN) {
			App->gui->DeleteFocus();
			focus = true;
		}
	}
	if (focus == true && text_input == false) {
		SDL_StartTextInput();
		text_input = true;
	}
	else if (focus == false && text_input == true) {
		SDL_StopTextInput();
		text_input = false;
	}
	if (focus == true) 
	{
		if (App->input->special_keys == specialkeys::Backspace) 
		{
			if (position > 0)
			{
				label = label.erase(position - 1, 1);
				position--;
			}
		}
		else if (App->input->special_keys == specialkeys::Left) {
			if (position > 0)
				position--;
		}
		else if (App->input->special_keys == specialkeys::Right) {
			if (position < label.size())
				position++;
		}
		else if (App->input->special_keys == specialkeys::Supr) {
			if (position > 0) 
			{
				label = label.erase(position - 1);
				position--;
			}
		}
		else if (App->input->special_keys == specialkeys::Home) {
			position = 0;
		}
		else if (App->input->special_keys == specialkeys::End) {
			position = label.size();
		}

	}
	UI::PreUpdate();
	return true;
}

bool TextInputUI::PostUpdate() {
	iPoint dif_sprite = { 0,0 };
	if (this->active) {
		App->render->DrawQuad(quad, red, green, blue, alpha, true, false);
	}

	SDL_Rect rect = { 0,0,0,0 };
	if (strcmp(label.c_str(), "")) {
		SDL_Texture* text = App->font->Print(label.c_str(), {255,255,255,255});
		SDL_QueryTexture(text, NULL, NULL, &rect.w, &rect.h);
		SDL_Rect sprite = UI::Check_Printable_Rect(rect, dif_sprite);
		if (this->active) App->render->Blit(text, quad.x + dif_sprite.x, quad.y + dif_sprite.y, &sprite, 0.0F);
		App->tex->UnLoad(text);
	}

	if (focus == true) 
	{
		std::string label_from_position = label;
		label_from_position.erase(label_from_position.begin() + position, label_from_position.end());
		int w, h;
		App->font->CalcSize(label_from_position.c_str(), w, h);
		App->render->DrawLine(quad.x + w, quad.y, quad.x + w, quad.y + quad.h, 0, 255, 255, 255, false);
	}
	UI::PostUpdate();



	return true;
}

//BUG: when moving with left or right keys and typing
void TextInputUI::ChangeLabel(std::string text) 
{
	std::string label_part_2, label_part_1;
	label_part_1 = label_part_2 = label;
	for (unsigned i = position; i < label.size(); i++) 
	{
		label_part_1.erase(position);
	}
	if (strcmp(label.c_str(), label_part_1.c_str())) 
	{
		for (int i = 0; i < position; i++) 
		{
			label_part_2 = label_part_2.erase(0);
		}
	}
	else {
		label_part_2 = "";
	}
	label_part_1 += text.c_str();
	label_part_1 += label_part_2;
	label = label_part_1;
	position++;
}

void TextInputUI::SetLabel(std::string text) 
{
	if (App->input->GetKey(SDL_SCANCODE_GRAVE) != KEY_DOWN) 
	{
		label = text.c_str();
		position += label.size() - 1;
	}
}

void TextInputUI::SetPositionToZero() {
	position = 0;
}