//Code by https://github.com/MayKoder

//Copyright(c) 2020 MayKoder

//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files(the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions :

//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.

//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#pragma once

#ifndef __j1QUADTREE_H__
#define __j1QUADTREE_H__

#include<list>
#include<vector>
#include"MaykMath.h"
#include"Entity.h"

#define QUADNODE_CHILD_NUMBER 4
#define MAX_ITEMS_IN_QUADDNODE 5
#define IN_RANGE_QUADTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

class QuadTree;
class Entity;
struct QuadNode
{
	QuadNode();
	~QuadNode();

	//Initialize node data
	void Init(QuadTree*, QuadNode*, int, int, int, int);

	//Positions
	int x, y, w, h;

	//Indicates if it's a leaf
	bool isDivided;

	//Pointers
	QuadTree* root;
	QuadNode* parent;

	std::vector<QuadNode> childNodes;

	//Data
	std::list<Entity*> data;

	//Sets the rect info
	void SetRect(int&, int&, int&, int&);

	//Returns the node rect
	Rect GetRect() { return {x, y, w, h}; };

	//Return a pointer to the data list
	const std::list<Entity*>* GetContent() { return &data; }

	//Subdivides the tree X number of times
	static void SubDivide(QuadNode&, int);
};

class QuadTree
{
public:

	QuadTree();
	~QuadTree();

	//Initialize tree data
	void Init(TreeType, int, int, int, int);

	QuadNode baseNode;
	TreeType type;

	int tile_width = 0;
	int tile_height = 0;
	bool displayTree;

	int lowest_height = 0;
	QuadNode* lowestNode = nullptr;

	//Adds an entity to the tree by positions and manages subdivisions
	void AddEntityToNode(Entity&, Point);

	//Loads all nodes overlaping with the point in a list
	void LoadNodesToList(std::list<QuadNode*>*, QuadNode*, Point, Point);

	//Finds the leaf inside a point
	void FindLowestNodeInPoint(QuadNode*, const Point&);
	
	//Checks off axis rect overlap
	static bool CheckNodeOverLap(Rect, Rect);

	//Transforms input to isometric space
	static Point CoordsToIsometricInt(Point, Point);

	//Deletes the tree if it’s a pointer
	void Clear();

};

#endif //__j1QUADTREE_H__