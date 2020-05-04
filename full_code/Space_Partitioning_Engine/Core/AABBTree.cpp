#include"AABBTree.h"

//Code by https://github.com/MayKoder


AABBNode::AABBNode() : isDivided(false), root(nullptr), parent(nullptr)
{
	//Positions
	minPos = { 0, 0 };
	maxPos = { 0, 0 };
	color = {0, 255, 0, 255};
}
void AABBNode::Init(AABBTree* s_root, AABBNode* s_parent, int A_x, int A_y, int B_x, int B_y)
{
	root = s_root;
	parent = s_parent;

	SetRect(A_x, A_y, B_x, B_y);

	childs.reserve(AABBNODE_CHILD_NUMBER);

	isDivided = false;

}

AABBNode::~AABBNode()
{
	//It's called by ~j1QuadTree and deletes all the childs of the base, entering in a recursive loop deleting all the childs of the tree
	childs.clear();
}

void AABBNode::SetRect(int A_x, int A_y, int B_x, int B_y)
{

	minPos.x = A_x;
	minPos.y = A_y;

	maxPos.x = B_x;
	maxPos.y = B_y;
}

void AABBNode::UpdateNodePoints()
{
	Entity* temp = nullptr;

	minPos = {(int)(*data.begin())->position.x, (int)(*data.begin())->position.y - (int)(*data.begin())->blitRect.y };
	maxPos = {(int)(*data.begin())->position.x + (*data.begin())->blitRect.x, (int)(*data.begin())->position.y};

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

void AABBNode::SubDivide(AABBNode& node)
{
	if (!node.isDivided)
	{
		for (int i = 0; i < AABBNODE_CHILD_NUMBER; i++)
		{
			node.childs.push_back(AABBNode());
		}

		node.childs[0].Init(node.root, &node, node.minPos.x, node.minPos.y, 0, 0);
		node.childs[1].Init(node.root, &node, node.maxPos.x, node.maxPos.y, 0, 0);

		for (std::list<Entity*>::iterator it = node.data.begin(); it != node.data.end(); it++)
		{
			float dMin = node.minPos.DistanceManhattan((Point)(*it)->position);
			float dMax = node.maxPos.DistanceManhattan((Point)(*it)->position);

			if (dMin < dMax || dMin == dMax)
			{
				node.childs[0].data.push_back((*it));
			}
			else if (dMin > dMax) 
			{
				node.childs[1].data.push_back((*it));
			}

		}

		node.isDivided = true;
	}
}

//////////////// QUAD TREE ////////////////
AABBTree::AABBTree() : type(TreeType::ORTHOGRAPHIC), lowestNode(nullptr)
{
}
void AABBTree::Init(TreeType s_type, int s_x, int s_y, int s_w, int s_h)
{
	type = s_type;
	//baseNode.Init(this, nullptr, s_x, s_y, s_w, s_h);
}
AABBTree::~AABBTree()
{
	//Deletes first node and calls ~QuadNode

}

//void AABBTree::FindLoadNodesToList(std::list<QuadNode*>* list, QuadNode* node, Point l2, Point r2)
//{
//
//	Rect rect = { node->x, node->y, node->w, node->h };
//
//	//TODO: 160 WHAT
//	Rect r = { l2.x, l2.y, r2.x, r2.y };
//	if (this->type == TreeType::ISOMETRIC)
//		r.w += node->root->lowest_height;
//
//	//OPT: Dumb cunt don't check all the nodes lmao
//	if (node->isDivided)
//	{
//		for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
//		{
//			FindLoadNodesToList(list, &node->childs[i], l2, r2);
//		}
//	}
//	else
//	{
//		if (QuadNodeOverLap(rect, r) || QuadNodeOverLap(r, rect))
//		{
//			list->push_back(node);
//		}
//	}
//
//}
//
//bool AABBTree::QuadNodeOverLap(Rect rect, Rect r)
//{
//	//OPT: Needs a big update, detection can't be hardcoded with a +350...
//	//Use off axis collision detection
//	//TODO: Use scale to avoid culling bugs when zooming in or out
//
//	if (r.x > rect.x&& r.x < rect.x + rect.w && r.y > rect.y&& r.y < rect.y + rect.h)
//		return true;
//
//	if (r.x + r.w > rect.x&& r.x + r.w < rect.x + rect.w && r.y + r.h > rect.y && r.y + r.h < rect.y + rect.h)
//		return true;
//
//	if (r.x > rect.x&& r.x  < rect.x + rect.w && r.y + r.h > rect.y&& r.y + r.h < rect.y + rect.h)
//		return true;
//
//	if (r.x + r.w > rect.x&& r.x + r.w < rect.x + rect.w && r.y > rect.y && r.y < rect.y + rect.h)
//		return true;
//
//	return false;
//
//}
//
//Point AABBTree::CoordsToIsometricInt(Point input, Point tileSize)
//{
//	Point ret;
//
//	float half_width = tileSize.x * 0.5f;
//	float half_height = tileSize.y * 0.5f;
//
//	ret.x = int((input.x / half_width + input.y / half_height) / 2) - 1;
//	ret.y = int((input.y / half_height - (input.x / half_width)) / 2);
//
//	return ret;
//}
//
//void AABBTree::AddEntityToNode(Entity& ent, Point p)
//{
//
//	//Figure lowest node out
//	FindLowestNodeInPoint(&baseNode, p);
//	if (lowestNode != nullptr)
//	{
//		lowestNode->data.push_back(&ent);
//		if (lowestNode->data.size() >= MAX_ITEMS_IN_NODE)
//		{
//			lowestNode->SubDivide(*lowestNode, 1);
//
//			Rect rect;
//			//For every data element set it to the new subdivision
//			for (std::list<Entity*>::iterator it = lowestNode->data.begin(); it != lowestNode->data.end(); it++)
//			{
//				for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
//				{
//					rect = lowestNode->childs[i].GetRect();
//					if (IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, { (int)(*it)->position.x, (int)(*it)->position.y }))
//					{
//						lowestNode->childs[i].data.push_back(*it);
//						break;
//					}
//				}
//			}
//
//
//		}
//	}
//}
//
////This MUST return the lowest node, and you MUST remove the lowestNode variable
//void AABBTree::FindLowestNodeInPoint(QuadNode* node, const Point& p)
//{
//	Rect rect = node->GetRect();
//	QuadNode* res = nullptr;
//	if (IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, p))
//	{
//
//		if (node->childs.size() != 0)
//		{
//			for (unsigned int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
//			{
//				FindLowestNodeInPoint(&node->childs[i], p);
//			}
//		}
//		else
//		{
//			lowestNode = node;
//		}
//	}
//}

void AABBTree::AddUnitToTree(Entity& ent)
{

	AABBNode* lowestNode = nullptr;
	//Fins lowest node in point
	lowestNode = FindLowestNode(&baseNode, (Point)ent.position);

	//Is point inside node?
	//is node divided? if not, this is the lowest node, 

	if (lowestNode != nullptr) 
	{

		AABBNode* back = lowestNode->parent;
		while (back != nullptr)
		{
			back->data.push_back(&ent);
			back = back->parent;
		}

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

	if (node.data.size() > 0) 
	{
		node.UpdateNodePoints();

		if (node.isDivided)
		{
			for (int i = 0; i < AABBNODE_CHILD_NUMBER; i++)
			{
				UpdateAllNodes(node.childs[i]);
			}
		}
	}
}

AABBNode* AABBTree::FindLowestNode(AABBNode* node, const Point p)
{
	if (node->isDivided)
	{
		//is it closer to A or B?
		float dMin = node->minPos.DistanceManhattan(p);
		float dMax = node->maxPos.DistanceManhattan(p);

		if (dMin < dMax || dMin == dMax)
		{
			return FindLowestNode(&node->childs[0], p);
		}
		else if (dMin > dMax)
		{
			return FindLowestNode(&node->childs[1], p);
		}
	}
	else
	{
		return node;
	}
}

void AABBTree::Clear()
{
	//Deletes and calls ~j1QuadTree
	delete this;
}