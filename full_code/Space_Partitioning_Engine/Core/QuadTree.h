#pragma once

//Code by https://github.com/MayKoder

#ifndef __j1QUADTREE_H__
#define __j1QUADTREE_H__

#include<list>
#include<vector>
#include"MaykMath.h"

//DELETE THIS AND MAKE IT A TEMPLATE
//TODO: You need to delete all use of entity and change it to a template
#include"Entity.h"

#define QUADNODE_CHILD_NUMBER 4
#define MAX_ITEMS_IN_NODE 5
#define IN_RANGE_QUADTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

class QuadTree;
class Entity;
struct QuadNode
{
	QuadNode();
	~QuadNode();

	void Init(QuadTree*, QuadNode*, int, int, int, int);

	//Positions
	int x, y, w, h;
	bool isDivided;

	//Pointers
	QuadTree* root;
	QuadNode* parent;
	std::vector<QuadNode> childs;

	//Data
	std::list<Entity*> data;

	void SetRect(int&, int&, int&, int&);

	Rect GetRect() { return {x, y, w, h}; };
	const std::list<Entity*>* GetContent() { return &data; }

	static void SubDivide(QuadNode&, int);
};

class QuadTree
{
public:

	QuadTree();
	~QuadTree();

	void Init(TreeType, int, int, int, int);

	QuadNode baseNode;
	TreeType type;

	int tile_width = 0;
	int tile_height = 0;

	int lowest_height = 0;
	QuadNode* lowestNode = nullptr;

	//Delete and free all the tree memory
	void Clear();
	void FindLoadNodesToList(std::list<QuadNode*>*, QuadNode*, Point, Point);
	
	void FindLowestNodeInPoint(QuadNode*, const Point&);
	
	static bool QuadNodeOverLap(Rect, Rect);
	static Point CoordsToIsometricInt(Point, Point);

	void AddEntityToNode(Entity&, Point);

};

#endif //__j1QUADTREE_H__