#pragma once

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceMesh.h"
#include "ResourceSkeleton.h"
#include "Timer.h"

struct DeformableMesh
{
	DeformableMesh(){}
		
	~DeformableMesh()
	{
	if (vertices != nullptr)
	{
		delete vertices;
	}
	if (normals != nullptr)
	{
		delete normals;
	}
	}
		
	uint idVertices = 0; // id in VRAM 
	uint numVertices = 0;
	float* vertices = nullptr;
		
	uint idNormals = 0; // id in VRAM
	float* normals = nullptr;

};


class CompMesh : public Component
{
public:
	CompMesh();

	~CompMesh();

	AABB enclosingBox;

	void Update(float dt);

	//Returns the center point of the mesh's enclosing box.
	float3 GetCenter() const;

	void DrawDebug() const;

	void OnEditor()override;

	void OnSave(Configuration& data) const;
	void OnLoad(Configuration& data);
	AABB GetEnclosingBox();
	void CreateEnclosingBox();

	void AddResourceByName(std::string filename);
	void AddResource(int uid);

	void AddResourceSkeletonByName(std::string filename);
	void AddResourceSkeleton(int uid);

	ResourceMesh* GetResourceMesh();

	void CreateDeformableMesh();
	void ResetDeformableMesh();
	void PlaceBones();
public:
	DeformableMesh* deformableMesh = nullptr;
	bool drawdebug = false;
	bool ChangingMesh = false;
	ResourceMesh* resourceMesh = nullptr;
	ResourceSkeleton* resourceskeleton = nullptr;
	bool bonesplaceds = false;
	Timer meshtimer;
	bool meshtimerstart = false;
	int frame = 0;
};