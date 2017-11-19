#pragma once

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceMesh.h"

class CompMesh : public Component
{
public:
	CompMesh();

	~CompMesh();

	AABB enclosingBox;

	void Update();

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
	ResourceMesh* GetResourceMesh();
public:
	bool drawdebug = false;
	bool ChangingMesh = false;
	ResourceMesh* resourceMesh = nullptr;
};