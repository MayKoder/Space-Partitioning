#pragma once

//Code by https://github.com/MayKoder

#ifndef __j1AABBTREE_H__
#define __j1AABBTREE_H__

#include<list>
#include<vector>
#include"MaykMath.h"

//DELETE THIS AND MAKE IT A TEMPLATE
//TODO: You need to delete all use of entity and change it to a template
#include"Entity.h"

#define AABBNODE_CHILD_NUMBER 2
#define MAX_ITEMS_IN_AABBNODE 6
#define IN_RANGE_AABBTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

class AABBTree;
class Entity;
struct AABBNode
{
	AABBNode();
	~AABBNode();

	void Init(AABBTree*, AABBNode*, int, int, int, int);

	//Positions
	Point minPos;
	Point maxPos;

	bool isDivided;

	//Pointers
	AABBTree* root;
	AABBNode* parent;
	std::vector<AABBNode> childs;

	//Data
	std::list<Entity*> data;
	Vector4 color;

	void SetRect(int, int, int, int);

	Rect GetRect() 
	{ 
			return {minPos.x, minPos.y, maxPos.x - minPos.x, maxPos.y - minPos.y};
	}
	std::list<Entity*>* GetData() 
	{
		return &data;
	}
	std::list<Entity*> GetDataValue() 
	{
		return data;
	}
	void UpdateNodePoints();
	//std::list<iPoint>* GetContent() { return &data; }

	static void SubDivide(AABBNode&);
};

class AABBTree
{
public:

	AABBTree();
	~AABBTree();

	void Init(TreeType, int, int, int, int);

	AABBNode baseNode;
	TreeType type;
	bool displayTree;

	AABBNode* lowestNode;

	//Delete and free all the tree memory
	void AddUnitToTree(Entity&);

	void UpdateAllNodes(AABBNode& node);
	AABBNode* FindLowestNode(AABBNode*, const Point);
	void LoadInterNodesToList(AABBNode*, std::list<AABBNode*>&);

	void Clear();
	//static bool QuadNodeOverLap(Rect, Rect);
	//static Point CoordsToIsometricInt(Point, Point);

};

#endif //__j1QUADTREE_H__