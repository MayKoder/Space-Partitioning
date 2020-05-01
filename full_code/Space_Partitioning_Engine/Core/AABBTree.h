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
#define IN_RANGE_AABBTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

class AABBTree;
class Entity;
struct AABBNode
{
	AABBNode();
	~AABBNode();

	void Init(AABBTree*, AABBNode*, float*, float*, float*, float*);

	//Positions
	pPoint minPos;
	pPoint maxPos;

	bool isDivided;

	//Pointers
	AABBTree* root;
	AABBNode* parent;
	std::vector<AABBNode> childs;

	//Data
	std::list<Entity*> data;

	void SetRect(float*, float*, float*, float*);

	Rect GetRect() 
	{ 
		if (minPos && maxPos) 
		{
			return {(int)*minPos.x, (int)*minPos.y, (int)*maxPos.x - (int)*minPos.x, (int)*maxPos.y - (int)*minPos.y};
		}
		else
		{
			return {0, 0, 0, 0};
		}
	}
	void UpdateNodePoints();
	//std::list<iPoint>* GetContent() { return &data; }

	//static void SubDivide(AABBNode&, int);
};

class AABBTree
{
public:

	AABBTree();
	~AABBTree();

	void Init(TreeType, int, int, int, int);

	AABBNode baseNode;
	TreeType type;

	//Delete and free all the tree memory
	void AddUnitToTree(Entity&);
	void Clear();
	//void FindLoadNodesToList(std::list<AABBNode*>*, AABBNode*, Point, Point);
	//static bool QuadNodeOverLap(Rect, Rect);
	//static Point CoordsToIsometricInt(Point, Point);

};

#endif //__j1QUADTREE_H__