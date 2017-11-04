#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"
#include "MathGeoLib.h"
#include "GeometryLoader.h"
#include "GameObject.h"
#include "Quadtree.h"

class ModuleEditor : public Module
{
public:

	ModuleEditor(Application* app, bool start_enable = true);
	~ModuleEditor();

	bool Init();
	bool Start();
	update_status Update(float dt);

	bool CleanUp() override;

	void Render();
	void ShowEditor();

	GameObject* GetRoot();
	Octree* GetQuadtree();
	GameObject* CreateNewGameObject(const char* path);
	GameObject* CastRay(const LineSegment& Segment, float Distance);

	void SelectGameObject(GameObject* selected);
	void TestRay(const LineSegment& Segment, float* Distance, GameObject* &Select);
public:
	Octree* Quadroot;
	GameObject* root = nullptr;
	GameObject* selected = nullptr;
	std::vector<GameObject*> Static_Vector;

};
