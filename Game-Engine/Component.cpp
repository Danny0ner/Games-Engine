#include "Component.h"
#include "GameObject.h"

Component::Component(ComponentType type, bool startActive) : type(type), active(startActive)
{}

Component::~Component()
{}

void Component::Enable()
{
	active = true;
}

void Component::Update(float dt)
{}

void Component::Disable()
{
	active = false;
}

void Component::OnEditor()
{}

void Component::SetName(const char* name)
{
	this->name.assign(name);
}

void Component::SetGameObject(GameObject * GO)
{
	myGO = GO;
}

ComponentType Component::GetType() const
{
	return type;
}

void Component::OnSave(Configuration & data) const
{
}

void Component::OnLoad(Configuration & data)
{
}

void Component::AddResource(int UID)
{
}

void Component::AddResourceByName(std::string filename)
{
}

GameObject * Component::getMyGO()
{
	if(myGO != nullptr)
	return myGO;
	else return nullptr;
}
