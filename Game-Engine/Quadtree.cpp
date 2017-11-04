#include "Application.h"
#include "Quadtree.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "MathGeoLib\MathGeoLib.h"
#include "MathGeoLib\Geometry\AABB.h"


#define MAX_OBJECTS 8
#define MIN_SIZE 1

OctreeNode::OctreeNode(const AABB& box) : box(box)
{
	parent = nullptr;
	for (int i = 0; i < 8; i++)
	{
		childs[i] = nullptr;
	}
}

OctreeNode::~OctreeNode()
{
	for (int i = 0; i < 8; i++)
	{
		delete childs[i];
		childs[i] = nullptr;
	}
}

bool OctreeNode::IsLeaf() const
{
	if (childs[0] == nullptr)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void OctreeNode::Insert(GameObject* toInsert)
{
	if (objects.size() < MAX_OBJECTS || box.Size().x <= MIN_SIZE)
	{
		objects.push_back(toInsert);
	}
	else
	{
		if (IsLeaf() == true)
		{
			CreateChilds();
		}
		RedistributeChilds();
	}
}

//TODO: Change function to an iterative one.
void OctreeNode::Remove(GameObject* toRemove)
{
	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end(); it++)
	{
		if ((*it) == toRemove)
		{
			objects.erase(it);
			return;
		}
	}
	if (IsLeaf() == false)
	{
		for (int i = 0; i < 8; i++)
		{
			childs[i]->Remove(toRemove);
		}
	}
}

void OctreeNode::CreateChilds()
{
	float			cube_sizex = box.HalfSize().x;
	float			cube_sizey = box.HalfSize().y;
	float			cube_sizez = box.HalfSize().z;
	math::AABB		temp_ab;

	//Calculate new AABB for each child

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x - cube_sizex,box.CenterPoint().y - cube_sizey,box.CenterPoint().z - cube_sizez };
	childs[0] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x + cube_sizex, box.CenterPoint().y - cube_sizey, box.CenterPoint().z - cube_sizez };
	childs[1] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x - cube_sizex, box.CenterPoint().y + cube_sizey, box.CenterPoint().z - cube_sizez };
	childs[2] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x - cube_sizex, box.CenterPoint().y - cube_sizey, box.CenterPoint().z + cube_sizez };
	childs[3] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x + cube_sizex, box.CenterPoint().y + cube_sizey,  box.CenterPoint().z + cube_sizez };
	childs[4] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x + cube_sizex, box.CenterPoint().y + cube_sizey, box.CenterPoint().z - cube_sizez };
	childs[5] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();;
	temp_ab.maxPoint = { box.CenterPoint().x + cube_sizex, box.CenterPoint().y - cube_sizey, box.CenterPoint().z + cube_sizez };
	childs[6] = new OctreeNode(temp_ab);

	temp_ab.minPoint = box.CenterPoint();
	temp_ab.maxPoint = { box.CenterPoint().x - cube_sizex, box.CenterPoint().y + cube_sizey, box.CenterPoint().z + cube_sizez };
	childs[7] = new OctreeNode(temp_ab);

	for (int i = 0; i < 8; i++)
	{
		childs[i]->parent = this;
	}
}

void OctreeNode::RedistributeChilds()
{
	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end();)
	{
		for (int i = 0; i < 8; i++)
		{
		
			CompMesh* tmp = (CompMesh*)(*it)->FindComponent(Component_Mesh);
			CompTransform* transf = (CompTransform*)(*it)->FindComponent(Component_Transform);
			if (tmp != nullptr)
			{
				AABB Enclosing_Box = tmp->enclosingBox;
				Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
				if (childs[i]->box.Intersects(Enclosing_Box));
				{
					childs[i]->Insert((*it));
				}
			}
		}
		it = objects.erase(it);
	}
}

void OctreeNode::DrawDebug(Color color) const
{
	if (IsLeaf() == true)
	{
		box.DrawDebug(color);
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			childs[i]->DrawDebug(color);
		}
	}
}

template<typename TYPE>
void OctreeNode::CollectIntersections(std::vector<GameObject*>& objects, const TYPE & primitive) const
{

	if (primitive.Intersects(box))
	{
		for (std::list<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			CompMesh* tmp = (CompMesh*)(*it)->FindComponent(Component_Mesh);
			CompTransform* transf = (CompTransform*)(*it)->FindComponent(Component_Transform);
			AABB Enclosing_Box = tmp->enclosingBox;
			Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
			if (tmp != nullptr)
			{
				if (primitive.Intersects(Enclosing_Box))
				{
					objects.push_back(*it);
				}
			}
		}
		for (int i = 0; i < 8; ++i)
			if (childs[i] != nullptr) childs[i]->CollectIntersections(objects, primitive);
	}
}


void OctreeNode::CollectIntersectionsLine(std::vector<GameObject*> &objects, const LineSegment& line) const
{

	if (line.Intersects(box))
	{
		for (std::list<GameObject*>::const_iterator it = this->objects.begin(); it != this->objects.end(); ++it)
		{
			CompMesh* tmp = (CompMesh*)(*it)->FindComponent(Component_Mesh);
			CompTransform* transf = (CompTransform*)(*it)->FindComponent(Component_Transform);
			AABB Enclosing_Box = tmp->enclosingBox;
			Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
			if (tmp != nullptr)
			{
				if (line.Intersects(Enclosing_Box))
				{
					objects.push_back(*it);
				}
			}
		}
		for (int i = 0; i < 8; ++i)
			if (childs[i] != nullptr) childs[i]->CollectIntersections(objects, line);
	}
}

Octree::Octree(const AABB& box)
{
	root = new OctreeNode(box);
}

Octree::~Octree()
{
	Clear();
}

void Octree::Insert(GameObject* toInsert)
{
	
	CompMesh* tmp = (CompMesh*)toInsert->FindComponent(Component_Mesh);
	CompTransform* transf = (CompTransform*)toInsert->FindComponent(Component_Transform);
	AABB Enclosing_Box = tmp->enclosingBox;
	Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
	if (root != nullptr && root->box.Contains(Enclosing_Box))
	{
		root->Insert(toInsert);
	}
}

void Octree::Remove(GameObject* toRemove)
{
	CompMesh* tmp = (CompMesh*)toRemove->FindComponent(Component_Mesh);
	CompTransform* transf = (CompTransform*)toRemove->FindComponent(Component_Transform);
	AABB Enclosing_Box = tmp->enclosingBox;
	Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
	if (root != nullptr && root->box.Contains(Enclosing_Box))
	{
		root->Remove(toRemove);
	}
}

void Octree::Clear()
{
	delete root;
	root = nullptr;
}

void Octree::DrawDebug(Color color) const
{
	root->DrawDebug(color);
}
