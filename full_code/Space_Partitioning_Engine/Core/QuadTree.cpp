#include"QuadTree.h"

//Code by https://github.com/MayKoder

QuadNode::QuadNode(QuadTree* s_root, QuadNode* s_parent, int s_x, int s_y, int s_w, int s_h)
{
	root = s_root;
	parent = s_parent;
	x = s_x; y = s_y;
	w = s_w; h = s_h;

	for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
	{
		childs[i] = nullptr;
	}
	isDivided = false;

}

QuadNode::~QuadNode()
{
	//It's called by ~j1QuadTree and deletes all the childs of the base, entering in a recursive loop deleting all the childs of the tree
	for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
	{
		if (childs[i])
			delete childs[i];
	}
}

void QuadNode::SetRect(int s_x, int s_y, int s_w, int s_h) 
{
	x = s_x;
	y = s_y;
	w = s_w;
	h = s_h;
}

void QuadNode::SubDivide(QuadNode* node, int divisionsLeft) 
{
	if (node->root->lowest_height > node->h)
		node->root->lowest_height = node->h;

	if (divisionsLeft > 0) 
	{
		if (!node->isDivided)
		{
			switch (node->root->type)
			{
			case NORMAL:
				node->childs[0] = new QuadNode(node->root, node, node->x, node->y, node->w / 2, node->h / 2);
				node->childs[1] = new QuadNode(node->root, node, node->x + (node->w / 2), node->y, node->w / 2, node->h / 2);

				node->childs[2] = new QuadNode(node->root, node, node->x, node->y + (node->h / 2), node->w / 2, node->h / 2);
				node->childs[3] = new QuadNode(node->root, node, node->x + (node->w / 2), node->y + (node->h / 2), node->w / 2, node->h / 2);
				break;

			case ISOMETRIC:
				node->childs[0] = new QuadNode(node->root, node, node->x, node->y, node->w / 2, node->h / 2);
				node->childs[1] = new QuadNode(node->root, node, node->x + (node->w / 4), node->y + (node->h / 4), node->w / 2, node->h / 2);

				node->childs[2] = new QuadNode(node->root, node, node->x - (node->w / 4), node->y + (node->h / 4), node->w / 2, node->h / 2);
				node->childs[3] = new QuadNode(node->root, node, node->x, node->y + (node->h / 2), node->w / 2, node->h / 2);
				break;
			}

			node->isDivided = true;



			for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
			{
				SubDivide(node->childs[i], divisionsLeft - 1);
			}
		}
	}

}

Rect QuadNode::GetRect() {
	return {x, y, w, h};
}

//////////////// QUAD TREE ////////////////
QuadTree::QuadTree(TreeType s_type, int s_x, int s_y, int s_w, int s_h) 
{
	type = s_type;
	baseNode = new QuadNode(this, nullptr, s_x, s_y, s_w, s_h);
	lowest_height = s_w;
}
QuadTree::~QuadTree() 
{
	//Deletes first node and calls ~QuadNode
	delete baseNode;
}

void QuadTree::FindLoadNodesToList(std::list<QuadNode*>* list, QuadNode* node, Point l2, Point r2)
{

	Rect rect = {node->x, node->y, node->w, node->h};

	//TODO: 160 WHAT
	Rect r = {l2.x, l2.y, r2.x, r2.y};
	if (this->type == TreeType::ISOMETRIC)
		r.w += node->root->lowest_height;

	if (node->isDivided) 
	{
		for (int i = 0; i < QUADNODE_CHILD_NUMBER; i++)
		{
			FindLoadNodesToList(list, node->childs[i], l2, r2);
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

void QuadTree::Clear() 
{
	//Deletes and calls ~j1QuadTree
	delete this;
}