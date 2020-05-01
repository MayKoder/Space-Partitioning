//#pragma once
//
////Code by https://github.com/MayKoder
//
//#ifndef __j1AABBTREE_H__
//#define __j1AABBTREE_H__
//
//#include<list>
//#include<vector>
//#include"MaykMath.h"
//
//#define AABBNODE_CHILD_NUMBER 2
//#define IN_RANGE_AABBTREE( value, min, max ) ( ((value) >= (min) && (value) <= (max)) ? 1 : 0 )
//
//enum TreeType
//{
//	ORTHOGRAPHIC,
//	ISOMETRIC
//};
//
//class AABBTree;
//struct AABBNode
//{
//	AABBNode();
//	~AABBNode();
//
//	void Init(AABBTree*, AABBNode*, int, int, int, int);
//
//	//Positions
//	int x, y, w, h;
//	bool isDivided;
//
//	//Pointers
//	AABBTree* root;
//	AABBNode* parent;
//	std::vector<AABBNode> childs;
//
//	//Data
//	//std::list<iPoint> data;
//
//	void SetRect(int&, int&, int&, int&);
//
//	Rect GetRect() { return { x, y, w, h }; };
//	//std::list<iPoint>* GetContent() { return &data; }
//
//	static void SubDivide(AABBNode&, int);
//};
//
//class AABBTree
//{
//public:
//
//	AABBTree();
//	~AABBTree();
//
//	void Init(TreeType, int, int, int, int);
//
//	AABBNode baseNode;
//	TreeType type;
//
//	int tile_width = 0;
//	int tile_height = 0;
//
//	int lowest_height = 0;
//
//	//Delete and free all the tree memory
//	void Clear();
//	void FindLoadNodesToList(std::list<AABBNode*>*, AABBNode*, Point, Point);
//	static bool QuadNodeOverLap(Rect, Rect);
//	static Point CoordsToIsometricInt(Point, Point);
//
//};
//
//#endif //__j1QUADTREE_H__