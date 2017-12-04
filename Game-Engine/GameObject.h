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

	void Update();
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
	int GetUID();
	float3 GetPosition();
protected:
	int uid;
	GameObject* parent;
	std::string name;
public:
	bool selected = false;
	bool Static = true;
	bool deletingchilds = false;
	bool deletingmyself = false;
};
