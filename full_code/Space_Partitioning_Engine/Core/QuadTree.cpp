#include"QuadTree.h"

//Code by https://github.com/MayKoder


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

	childs.reserve(QUADNODE_CHILD_NUMBER);

	isDivided = false;

}

QuadNode::~QuadNode()
{
	//It's called by ~j1QuadTree and deletes all the childs of the base, entering in a recursive loop deleting all the childs of the tree
	childs.clear();
}

void QuadNode::SetRect(int& s_x, int& s_y, int& s_w, int& s_h)
{
	x = s_x;
	y = s_y;
	w = s_w;
	h = s_h;
}

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
				node.childs.push_back(QuadNode());
			}
			switch (node.root->type)
			{
			case ORTHOGRAPHIC:
				node.childs[0].Init(node.root, &node, node.x, node.y, node.w / 2, node.h / 2);
				node.childs[1].Init(node.root, &node, node.x + (node.w / 2), node.y, node.w / 2, node.h / 2);
								
				node.childs[2].Init(node.root, &node, node.x, node.y + (node.h / 2), node.w / 2, node.h / 2);
				node.childs[3].Init(node.root, &node, node.x + (node.w / 2), node.y + (node.h / 2), node.w / 2, node.h / 2);
				break;			
								 
			case ISOMETRIC:		 
				node.childs[0].Init(node.root, &node, node.x, node.y, node.w / 2, node.h / 2);
				node.childs[1].Init(node.root, &node, node.x + (node.w / 4), node.y + (node.h / 4), node.w / 2, node.h / 2);
									 
				node.childs[2].Init(node.root, &node, node.x - (node.w / 4), node.y + (node.h / 4), node.w / 2, node.h / 2);
				node.childs[3].Init(node.root, &node, node.x, node.y + (node.h / 2), node.w / 2, node.h / 2);
				break;
			}

			node.isDivided = true;



			for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				SubDivide(node.childs[i], divisionsLeft - 1);
			}
		}
	}

}

//////////////// QUAD TREE ////////////////

//TODO IMPORTANT: When checking close entities, check all childs, not only the data inside one child but the data inside all 4 childs
//to avoid an entity being close to the edge of a node and not detecting another entity in the next child node
//Check if the point you are looking for is inside the current node, if not, get the new node
QuadTree::QuadTree() : type(TreeType::ORTHOGRAPHIC), lowest_height(0), tile_width(0), tile_height(0), displayTree(false)
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

void QuadTree::FindLoadNodesToList(std::list<QuadNode*>* list, QuadNode* node, Point l2, Point r2)
{

	Rect rect = {node->x, node->y, node->w, node->h};

	Rect r = {l2.x, l2.y, r2.x, r2.y};
	if (this->type == TreeType::ISOMETRIC)
		r.w += node->root->lowest_height;

	//OPT: Dumb cunt don't check all the nodes lmao
	if (node->isDivided) 
	{
		for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
		{
			FindLoadNodesToList(list, &node->childs[i], l2, r2);
		}
	}
	else
	{
		if (QuadNodeOverLap(rect, r) || QuadNodeOverLap(r, rect))
		{
			list->push_back(node);
		}
	}

}

bool QuadTree::QuadNodeOverLap(Rect rect, Rect r)
{
	//OPT: Needs a big update, detection can't be hardcoded with a +350...
	//Use off axis collision detection
	//TODO: Use scale to avoid culling bugs when zooming in or out

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

		QuadNode* back = lowestNode->parent;
		while (back != nullptr)
		{
			back->data.push_back(&ent);
			back = back->parent;
		}

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
					rect = lowestNode->childs[i].GetRect();
					if (MaykMath::IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, {(int)(*it)->position.x, (int)(*it)->position.y }))
					{
						lowestNode->childs[i].data.push_back(*it);
						break;
					}
				}
			}


		}
		lowestNode = nullptr;
	}
}

//This MUST return the lowest node, and you MUST remove the lowestNode variable
//TODO: Adding an element also means adding it to parents
void QuadTree::FindLowestNodeInPoint(QuadNode* node, const Point& p)
{
	Rect rect = node->GetRect();
	QuadNode* res = nullptr;
	if (MaykMath::IsPointInsideOffAxisRectangle({ rect.x, rect.y }, { rect.x + rect.w / 2, rect.y + rect.h / 2 }, { rect.x - rect.w / 2, rect.y + rect.h / 2 }, { rect.x, rect.y + rect.h }, p))
	{

		if (node->childs.size() != 0) 
		{
			for (unsigned int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				FindLowestNodeInPoint(&node->childs[i], p);
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