#pragma once

#include <string>
#include "SerializeConfig.h"
class GameObject;

enum ComponentType
{
	Component_Transform,
	Component_Mesh,
	Component_Material,
	Component_Camera
};

class Component
{
public:
	Component(ComponentType type, bool startActive = true);
	virtual ~Component();

	virtual void Enable();
	virtual void Update();
	virtual void Disable();
	virtual void OnEditor();

	void SetName(const char* name);
	void SetGameObject(GameObject* GO);
	ComponentType GetType() const;
	virtual void OnSave(Configuration& data) const;
	virtual void OnLoad(Configuration& data);
protected:
	GameObject* myGO = nullptr;
	std::string name;
	bool active;
	ComponentType type;
};
