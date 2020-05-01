#ifndef __BUILDING_H__
#define __BUILDING_H__

#include "Entity.h"
#include"EntityManager.h"
#include "j1Timer.h"

struct BuildingInfo;

class Building: public Entity
{
public:
	Building();
	~Building();
	
	const char* GetDescription();
	BuildingType GetBuildingType() { return buildingType; }
	void Init(iPoint pos);

	bool Draw(float dt) override;

private:

	bool Awake(pugi::xml_node&);
	bool Update(float dt) override;

	std::string description;
	

	//Settigns
	BuildingType buildingType;
	int tileLenght;
	SDL_Rect original_spriteRect;
};

#endif // !__BUILDING_H__
