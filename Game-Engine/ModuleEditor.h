#pragma once
#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "GeometryLoader.h"
#include "GameObject.h"
#include "Octree.h"

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
	void TestRay(const LineSegment& Segment, float &Distance, GameObject* &Select);

	void CollectIntersectionsLineDynamicObjects(std::vector<GameObject*>& objects, const LineSegment & line) const;

	void CollectIntersectionsFrustumDynamicObjects(std::vector<GameObject*>& objects, const Frustum & line) const;

	void SerializeScene(const char* filename);
	void LoadScene(const char * fileTitle);
	void LockInput();
	void UnlockInput();
	bool IsInputLocked();

	void ReCreateQuadtree();

public:
	bool LockedInput;
	Octree* Quadroot;
	GameObject* root = nullptr;
	GameObject* selected = nullptr;
	std::vector<GameObject*> Static_Vector;
	std::vector<GameObject*> Dynamic_Vector;
	uint StaticVecSize = 0;
};
