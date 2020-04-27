#pragma once

//Code by https://github.com/MayKoder

#ifndef __j1QUADTREE_H__
#define __j1QUADTREE_H__

#include<list>
#include<vector>
#include"p2Point.h"

#define QUADNODE_CHILD_NUMBER 4
#define IN_RANGE_QUADTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

enum TreeType
{
	ORTHOGRAPHIC,
	ISOMETRIC
};
struct Point
{
	int x, y;
};
struct Rect
{
	int x, y, w, h;
};

class QuadTree;
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
	std::list<iPoint> data;

	void SetRect(int&, int&, int&, int&);

	Rect GetRect() { return {x, y, w, h}; };
	std::list<iPoint>* GetContent() { return &data; }

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

	//Delete and free all the tree memory
	void Clear();
	void FindLoadNodesToList(std::list<QuadNode*>*, QuadNode*, Point, Point);
	static bool QuadNodeOverLap(Rect, Rect);
	static Point CoordsToIsometricInt(Point, Point);

};

#endif //__j1QUADTREE_H__