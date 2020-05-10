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

#include"AABBTree.h"

AABBNode::AABBNode() : isDivided(false), root(nullptr), parent(nullptr)
{
	//Positions
	minPos = { -10, -10};
	maxPos = { 0, 0 };
	color = {0, 255, 0, 255};
}
void AABBNode::Init(AABBTree* s_root, AABBNode* s_parent, int A_x, int A_y, int B_x, int B_y)
{
	root = s_root;
	parent = s_parent;

	SetRect(A_x, A_y, B_x, B_y);

	childNodes.reserve(AABBNODE_CHILD_NUMBER);

	isDivided = false;

}

AABBNode::~AABBNode()
{
	//It's called by ~j1QuadTree and deletes all the children of the base, entering in a recursive loop deleting all the children of the tree
	childNodes.clear();
}

void AABBNode::SetRect(int A_x, int A_y, int B_x, int B_y)
{

	minPos.x = A_x;
	minPos.y = A_y;

	maxPos.x = B_x;
	maxPos.y = B_y;
}

//TODO 9: Take some time to understand the code
void AABBNode::UpdateNode()
{

	if (data.size() != 0) 
	{
		Entity* temp = nullptr;

		minPos = { (int)(*data.begin())->position.x, (int)(*data.begin())->position.y - (int)(*data.begin())->blitRect.y };
		maxPos = { (int)(*data.begin())->position.x + (*data.begin())->blitRect.x, (int)(*data.begin())->position.y };

		for (std::list<Entity*>::iterator it = data.begin(); it != data.end(); it++)
		{
			temp = (*it);
			if (temp->position.y - temp->blitRect.y < minPos.y)
			{
				minPos.y = temp->position.y - temp->blitRect.y;
			}
			if (temp->position.x < minPos.x)
			{
				minPos.x = temp->position.x;
			}

			if (temp->position.y > maxPos.y)
			{
				maxPos.y = temp->position.y;
			}
			if (temp->position.x + temp->blitRect.x > maxPos.x)
			{
				maxPos.x = temp->position.x + temp->blitRect.x;
			}
		}
	}
}

//TODO 6.1: Complete the missing code in the AABBTree::SubDivide() method
void AABBNode::SubDivide(AABBNode& node)
{
	if (!node.isDivided)
	{
		for (int i = 0; i < AABBNODE_CHILD_NUMBER; i++)
		{
			node.childNodes.push_back(AABBNode());
		}

		node.childNodes[0].Init(node.root, &node, node.minPos.x, node.minPos.y, node.minPos.x, node.minPos.y);
		node.childNodes[1].Init(node.root, &node, node.maxPos.x, node.maxPos.y, node.maxPos.x, node.maxPos.y);

		for (std::list<Entity*>::iterator it = node.data.begin(); it != node.data.end(); it++)
		{
			float dMin = node.minPos.DistanceManhattan((Point)(*it)->position);
			float dMax = node.maxPos.DistanceManhattan((Point)(*it)->position);

			if (dMin < dMax || dMin == dMax)
			{
				node.childNodes[0].data.push_back((*it));
			}
			else if (dMin > dMax) 
			{
				node.childNodes[1].data.push_back((*it));
			}

		}
		node.isDivided = true;
		node.data.clear();
	}
}

//////////////// QUAD TREE ////////////////
AABBTree::AABBTree() : displayTree(true)
{
}
void AABBTree::Init(int s_x, int s_y, int s_w, int s_h)
{
	//baseNode.Init(this, nullptr, s_x, s_y, s_w, s_h);
}
AABBTree::~AABBTree()
{
	//Deletes first node and calls ~QuadNode
}

void AABBTree::AddUnitToTree(Entity& ent)
{

	AABBNode* lowestNode = nullptr;
	//Fins lowest node in point
	lowestNode = FindLowestNodeInPoint(&baseNode, (Point)ent.position);

	//Is point inside node?
	//is node divided? if not, this is the lowest node
	if (lowestNode != nullptr) 
	{

		lowestNode->data.push_back(&ent);
		if (lowestNode->data.size() >= MAX_ITEMS_IN_AABBNODE)
		{
			//Subdivide
			lowestNode->SubDivide(*lowestNode);
		}
	}
}

void AABBTree::UpdateAllNodes(AABBNode& node)
{
	
	if (node.isDivided) 
	{
		for (int i = 0; i < AABBNODE_CHILD_NUMBER; i++)
		{
			UpdateAllNodes(node.childNodes[i]);
		}

		node.minPos = MaykMath::GetMinPoint(node.childNodes[0].minPos, node.childNodes[1].minPos);
		node.maxPos = MaykMath::GetMaxPoint(node.childNodes[0].maxPos, node.childNodes[1].maxPos);
	}
	else
	{
		node.UpdateNode();
	}
}

AABBNode* AABBTree::FindLowestNodeInPoint(AABBNode* node, const Point p)
{
	if (node->isDivided)
	{
		//is it closer to A or B?
		float dMin = node->minPos.DistanceManhattan(p);
		float dMax = node->maxPos.DistanceManhattan(p);

		if (dMin < dMax || dMin == dMax)
		{
			return FindLowestNodeInPoint(&node->childNodes[0], p);
		}
		else if (dMin > dMax)
		{
			return FindLowestNodeInPoint(&node->childNodes[1], p);
		}
	}
	else
	{
		return node;
	}
}

void AABBTree::LoadLeavesToList(AABBNode* node, std::list<AABBNode*>& list)
{

	if (node->isDivided) 
	{
		LoadLeavesToList(&node->childNodes[0], list);
		LoadLeavesToList(&node->childNodes[1], list);
	}
	else
	{
		list.push_back(node);
	}
}

void AABBTree::LoadLeafNodesInsideRect(AABBNode* node, std::vector<AABBNode*>& vec, Rect& collider)
{
	if (MaykMath::CheckRectCollision(node->GetRect(), collider))
	{
		if (node->isDivided) 
		{
			LoadLeafNodesInsideRect(&node->childNodes[0], vec, collider);
			LoadLeafNodesInsideRect(&node->childNodes[1], vec, collider);
		}
		else
		{
			vec.push_back(node);
		}
	}
}

void AABBTree::Clear()
{
	//Deletes and calls ~j1QuadTree
	delete this;
}