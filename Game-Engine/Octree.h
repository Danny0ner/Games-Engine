#pragma once

#include <list>

class OctreeNode
{
public:
	OctreeNode(const AABB& box);
	virtual ~OctreeNode();

	bool IsLeaf() const;

	void Insert(GameObject* toInsert);
	void Remove(GameObject* toRemove);
	void CreateChilds();
	void RedistributeChilds();
	void CollectIntersectionsLine(std::vector<GameObject*> &objects, const LineSegment&) const;
	void DrawDebug(Color color) const;

	void CollectIntersectionsFrustum(std::vector<GameObject*>& objects, const Frustum& primitive) const;

public:
	bool childscreateds = false;
	AABB box;
	std::list<GameObject*> objects;
	OctreeNode* parent;
	OctreeNode* childs[8];
};

class Octree
{
public:
	Octree(const AABB& box);
	virtual ~Octree();

	void Insert(GameObject* toInsert);
	void Remove(GameObject* toRemove);
	void Clear();

	void DrawDebug(Color color) const;

public:
	OctreeNode* root = nullptr;
};

