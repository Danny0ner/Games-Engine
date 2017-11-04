#pragma once
#include "Component.h"
#include "MathGeoLib\Math\float3.h"
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
	void Move(float3 lastpos,float3 newPos);

	GameObject * FindUIDGameObject(int toFind);

	void OnSerialize(Configuration & dataToSave) const;
	void Deserialize(Configuration & dataToSave);
	Component* FindComponent(ComponentType type) const;

	std::vector<Component*> components;
	std::vector<GameObject*> childs;

	int GetUID();
protected:
	int uid;
	GameObject* parent;
	std::string name;
public:
	bool selected = false;
	bool Static = true;
};
