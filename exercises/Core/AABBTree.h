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

#ifndef __j1AABBTREE_H__
#define __j1AABBTREE_H__

#include<list>
#include<vector>
#include"MaykMath.h"
#include"Entity.h"

#define AABBNODE_CHILD_NUMBER 2
#define MAX_ITEMS_IN_AABBNODE 4
#define IN_RANGE_AABBTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )

class AABBTree;
class Entity;
struct AABBNode
{

	//Constructor & Destructor
	AABBNode();
	~AABBNode();

	//Initialize node data
	void Init(AABBTree*, AABBNode*, int, int, int, int);

	//Min and Max node positions that construct the rect
	Point minPos;
	Point maxPos;

	//Indicates if the node is a leaf or a parent
	bool isDivided;

	//A pointer to the baseNode or root
	AABBTree* root;

	//A pointer to the parent
	AABBNode* parent;

	//Data holded by this node
	std::list<Entity*> data;
	Vector4 color;

	//Vector with the children info (size == 0 if is not divided)
	std::vector<AABBNode> childNodes;

	void SetRect(int, int, int, int);

	//Returns the node rect
	Rect GetRect() 
	{ 
			return {minPos.x, minPos.y, maxPos.x - minPos.x, maxPos.y - minPos.y};
	}

	//Returns a pointer to the node data list
	std::list<Entity*>* GetData() 
	{
		return &data;
	}

	//Returns a copy of the node data list
	std::list<Entity*> GetDataValue() 
	{
		return data;
	}

	//Updates the node min and max positions
	void UpdateNode();

	//Creates a node subdivision and distributes the old node data
	static void SubDivide(AABBNode&);
};

class AABBTree
{
public:

	//Constructor & Destructor
	AABBTree();
	~AABBTree();

	//Base node or root of the tree
	AABBNode baseNode;

	//Display variable, debug info won't be rendered if this is false
	bool displayTree;

	//Initialize tree data
	void Init(int, int, int, int);

	//Adds a unit to the tree by positions and manages subdivisions
	void AddUnitToTree(Entity&);

	//Updates all tree nodes with recursivity
	void UpdateAllNodes(AABBNode& node);

	//Returns a pointer to the lowest node inside a point
	AABBNode* FindLowestNodeInPoint(AABBNode*, const Point);

	//Loads all the leaves in a list
	void LoadLeavesToList(AABBNode*, std::list<AABBNode*>&);

	//Loads all leaves inside the rect
	void LoadLeafNodesInsideRect(AABBNode*, std::vector<AABBNode*>&, Rect& collider);

	//Deletes the tree if it’s a pointer
	void Clear();

};

#endif //__j1QUADTREE_H__