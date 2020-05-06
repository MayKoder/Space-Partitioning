#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include "j1Timer.h"
#include"j1Map.h"

struct BuildingInfo;

class Building: public Entity
{
public:
	Building();
	~Building();
	
	const char* GetDescription();
	void Init(iPoint pos, SDL_Texture* s_tex);

	bool Draw(float dt) override;

private:

	bool Awake(pugi::xml_node&);
	bool Update(float dt) override;

	std::string description;
	

	//Settigns
	SDL_Texture* tex;
	int tileLenght;
	SDL_Rect original_spriteRect;
};

#endif // !__BUILDING_H__
