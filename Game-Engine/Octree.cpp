#include "Application.h"
#include "Octree.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "MathGeo\MathGeoLib.h"
#include "MathGeo\Geometry\AABB.h"


#define MAX_OBJECTS 4
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
	if (childs[0] == NULL)
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
	if (objects.size() < MAX_OBJECTS)
	{
		objects.push_back(toInsert);
	}
	else
	{
		if (childscreateds == false)
		{
			CreateChilds();
			childscreateds = true;
		}
		objects.push_back(toInsert);
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
	AABB childBox;

	float3 boxMin = this->box.minPoint;
	float3 boxMax = this->box.maxPoint;
	float3 boxHalfSize = this->box.HalfSize();

	//Child 0
	childBox.minPoint = float3(boxMin.x, boxMin.y, boxMin.z);
	childBox.maxPoint = float3(boxMax.x - boxHalfSize.x, boxMax.y - boxHalfSize.y, boxMax.z - boxHalfSize.z);

	childs[0] = new OctreeNode(childBox);

	//Child 1
	childBox.minPoint = float3(boxMin.x, boxMin.y + boxHalfSize.y, boxMin.z);
	childBox.maxPoint = float3(boxMax.x - boxHalfSize.x, boxMax.y, boxMax.z - boxHalfSize.z);

	childs[1] = new OctreeNode(childBox);

	//Child 2
	childBox.minPoint = float3(boxMin.x, boxMin.y, boxMin.z + boxHalfSize.z);
	childBox.maxPoint = float3(boxMax.x - boxHalfSize.x, boxMax.y - boxHalfSize.y, boxMax.z);

	childs[2] = new OctreeNode(childBox);

	//Child 3
	childBox.minPoint = float3(boxMin.x, boxMin.y + boxHalfSize.y, boxMin.z + boxHalfSize.z);
	childBox.maxPoint = float3(boxMax.x - boxHalfSize.x, boxMax.y, boxMax.z);

	childs[3] = new OctreeNode(childBox);

	//Child 4
	childBox.minPoint = float3(boxMin.x + boxHalfSize.x, boxMin.y, boxMin.z);
	childBox.maxPoint = float3(boxMax.x, boxMax.y - boxHalfSize.y, boxMax.z - boxHalfSize.z);

	childs[4] = new OctreeNode(childBox);

	//Child 5
	childBox.minPoint = float3(boxMin.x + boxHalfSize.x, boxMin.y + boxHalfSize.y, boxMin.z);
	childBox.maxPoint = float3(boxMax.x, boxMax.y, boxMax.z - boxHalfSize.z);

	childs[5] = new OctreeNode(childBox);

	//Child 6
	childBox.minPoint = float3(boxMin.x + boxHalfSize.x, boxMin.y, boxMin.z + boxHalfSize.z);
	childBox.maxPoint = float3(boxMax.x, boxMax.y - boxHalfSize.y, boxMax.z);

	childs[6] = new OctreeNode(childBox);

	//Child 7
	childBox.minPoint = float3(boxMin.x + boxHalfSize.x, boxMin.y + boxHalfSize.y, boxMin.z + boxHalfSize.z);
	childBox.maxPoint = float3(boxMax.x, boxMax.y, boxMax.z);

	childs[7] = new OctreeNode(childBox);
	for (int i = 0; i < 8; i++)
	{
		childs[i]->parent = this;
	}
}

void OctreeNode::RedistributeChilds()
{
	for (std::list<GameObject*>::iterator it = objects.begin(); it != objects.end();)
	{
		CompMesh* tmp = (CompMesh*)(*it)->FindComponent(Component_Mesh);
		CompTransform* transf = (CompTransform*)(*it)->FindComponent(Component_Transform);
		if (tmp != nullptr)
		{
			AABB Enclosing_Box = tmp->enclosingBox;
			Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());


			bool intersect[8];
			for (int i = 0; i < 8; i++)	intersect[i] = childs[i]->box.Intersects(Enclosing_Box);

			if (intersect[0] && intersect[1] && intersect[2] && intersect[3] && intersect[4] && intersect[5] && intersect[6] && intersect[7]) it++;

			else
			{
				for (int i = 0; i < 8; i++)
				{
					if (childs[i]->box.Intersects(Enclosing_Box)) childs[i]->Insert(*it);
				}
				it = objects.erase(it);
			}
		}
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
