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

#include"QuadTree.h"


QuadNode::QuadNode() : isDivided(false), root(nullptr), parent(nullptr)
{
	//Positions
	x = y = w = h = 0;
}
void QuadNode::Init(QuadTree* s_root, QuadNode* s_parent, int s_x, int s_y, int s_w, int s_h)
{
	root = s_root;
	parent = s_parent;
	x = s_x; y = s_y;
	w = s_w; h = s_h;

	childNodes.reserve(QUADNODE_CHILD_NUMBER);

	isDivided = false;

}

QuadNode::~QuadNode()
{
	//It's called by ~j1QuadTree and deletes all the children of the base, entering in a recursive loop deleting all the children of the tree
	childNodes.clear();
}

void QuadNode::SetRect(int& s_x, int& s_y, int& s_w, int& s_h)
{
	x = s_x;
	y = s_y;
	w = s_w;
	h = s_h;
}

//TODO 6.2: Complete the missing code in the QuadTree::SubDivide() method
void QuadNode::SubDivide(QuadNode& node, int divisionsLeft)
{
	if (node.root->lowest_height > node.h)
		node.root->lowest_height = node.h;

	if (divisionsLeft > 0) 
	{
		if (!node.isDivided)
		{
			for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				node.childNodes.push_back(QuadNode());
			}
			switch (node.root->type)
			{
			case ORTHOGRAPHIC:
				node.childNodes[0].Init(node.root, &node, node.x, node.y, node.w / 2, node.h / 2);
				node.childNodes[1].Init(node.root, &node, node.x + (node.w / 2), node.y, node.w / 2, node.h / 2);
								
				node.childNodes[2].Init(node.root, &node, node.x, node.y + (node.h / 2), node.w / 2, node.h / 2);
				node.childNodes[3].Init(node.root, &node, node.x + (node.w / 2), node.y + (node.h / 2), node.w / 2, node.h / 2);
				break;			
								 
			case ISOMETRIC:		 
				node.childNodes[0].Init(node.root, &node, node.x, node.y, node.w / 2, node.h / 2);
				node.childNodes[1].Init(node.root, &node, node.x + (node.w / 4), node.y + (node.h / 4), node.w / 2, node.h / 2);
									 
				node.childNodes[2].Init(node.root, &node, node.x - (node.w / 4), node.y + (node.h / 4), node.w / 2, node.h / 2);
				node.childNodes[3].Init(node.root, &node, node.x, node.y + (node.h / 2), node.w / 2, node.h / 2);
				break;
			}

			node.isDivided = true;

			for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				SubDivide(node.childNodes[i], divisionsLeft - 1);
			}
		}
	}

}

//////////////// QUAD TREE ////////////////
QuadTree::QuadTree() : type(TreeType::ORTHOGRAPHIC), lowest_height(0), tile_width(0), tile_height(0), displayTree(true)
{
}
void QuadTree::Init(TreeType s_type, int s_x, int s_y, int s_w, int s_h) 
{
	type = s_type;
	baseNode.Init(this, nullptr, s_x, s_y, s_w, s_h);
	lowest_height = s_w;
}
QuadTree::~QuadTree() 
{
	//Deletes first node and calls ~QuadNode

}

void QuadTree::LoadNodesToList(std::list<QuadNode*>* list, QuadNode* node, Point l2, Point r2)
{

	Rect rect = {node->x, node->y, node->w, node->h};

	Rect r = {l2.x, l2.y, r2.x, r2.y};
	if (this->type == TreeType::ISOMETRIC)
		r.w += node->root->lowest_height;

	if (CheckNodeOverLap(rect, r) || CheckNodeOverLap(r, rect)) 
	{
		if (node->isDivided)
		{
			for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				LoadNodesToList(list, &node->childNodes[i], l2, r2);
			}
		}
		else
		{
			list->push_back(node);
		}
	}
}

bool QuadTree::CheckNodeOverLap(Rect rect, Rect r)
{
	if (r.x > rect.x && r.x < rect.x + rect.w && r.y > rect.y && r.y < rect.y + rect.h)
		return true;

	if (r.x + r.w > rect.x && r.x + r.w < rect.x + rect.w && r.y + r.h > rect.y && r.y+ r.h < rect.y + rect.h)
		return true;

	if (r.x > rect.x && r.x  < rect.x + rect.w && r.y + r.h > rect.y && r.y + r.h < rect.y + rect.h)
		return true;

	if (r.x + r.w > rect.x && r.x + r.w < rect.x + rect.w && r.y > rect.y && r.y < rect.y + rect.h)
		return true;

	return false;
}

Point QuadTree::CoordsToIsometricInt(Point input, Point tileSize) 
{
	Point ret;

	float half_width = tileSize.x * 0.5f;
	float half_height = tileSize.y * 0.5f;

	ret.x = int((input.x / half_width + input.y / half_height) / 2) - 1;
	ret.y = int((input.y / half_height - (input.x / half_width)) / 2);

	return ret;
}

void QuadTree::AddEntityToNode(Entity& ent, Point p)
{
	if (lowestNode != nullptr) 
	{
		lowestNode->data.push_back(&ent);
		if (lowestNode->data.size() >= MAX_ITEMS_IN_QUADDNODE)
		{
			lowestNode->SubDivide(*lowestNode, 1);

			Rect rect;
			//For every data element set it to the new subdivision
			for (std::list<Entity*>::iterator it = lowestNode->data.begin(); it != lowestNode->data.end(); it++)
			{
				for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
				{
					rect = lowestNode->childNodes[i].GetRect();
					if (MaykMath::IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, {(int)(*it)->position.x, (int)(*it)->position.y }))
					{
						lowestNode->childNodes[i].data.push_back(*it);
						break;
					}
				}
			}
			lowestNode->data.clear();

		}
		lowestNode = nullptr;
	}
}

//This CAN'T return the lowestNode and it must be a class variable because there is the option that a point is outside a node
//and we can't return nullptr or node.
void QuadTree::FindLowestNodeInPoint(QuadNode* node, const Point& p)
{
	Rect rect = node->GetRect();
	QuadNode* res = nullptr;
	if (MaykMath::IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, p))
	{

		if (node->isDivided) 
		{
			for (unsigned int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				FindLowestNodeInPoint(&node->childNodes[i], p);
			}
		}
		else
		{
			lowestNode = node;
		}
	}
}

void QuadTree::Clear() 
{
	//Deletes and calls ~j1QuadTree
	delete this;
}