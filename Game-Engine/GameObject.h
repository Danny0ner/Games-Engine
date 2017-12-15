#pragma once
#include "Component.h"
#include "MathGeo\Math\float3.h"
#include "SerializeConfig.h"
#include <string>
#include <vector>

class GameObject
{
public:
	GameObject(GameObject* parent = nullptr);
	~GameObject();

	void Update(float dt);
	void AddChild(GameObject* child);
	void DeleteChilds();
	void AddComponent(Component* component);
	void SetName(const char* name);
	void SetParent(GameObject* parent);
	GameObject* GetParent();
	void OnEditor();
	void ShowInspector();
	std::string Getname() const;

	GameObject * FindUIDGameObject(int toFind);

	void OnSerialize(Configuration & dataToSave) const;
	void Deserialize(Configuration & dataToSave);
	Component* FindComponent(ComponentType type) const;
	void ChangeStaticState();

	std::vector<Component*> components;
	std::vector<GameObject*> childs;
	void DeleteChild(GameObject* todelete);
    void FindSiblingOrChildGameObjectWithName(const char * name, GameObject * &gameobj);
	void FindChildGameObjectWithName(const char * name, GameObject * &gameobj);
	void DrawSkeletonDebug();
	int GetUID();
	float3 GetPosition();
	float3 GetScale();
protected:
	int uid;
	GameObject* parent;
	std::string name;
public:
	bool selected = false;
	bool Static = false;
	bool deletingchilds = false;
	bool deletingmyself = false;
	bool FinishedCreated = false;
};
