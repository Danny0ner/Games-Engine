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

	void AddResource(int uid);
public:
	bool drawdebug = false;
	ResourceMesh* resourceMesh = nullptr;
};