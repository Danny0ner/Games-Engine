#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "CompMesh.h"

GameObject::GameObject(GameObject* parent): parent(parent)
{
	name = "Game Object";
}
GameObject::~GameObject()
{
	while (!childs.empty())
	{
		delete childs.back();
		childs.pop_back();
	}
	childs.clear();
	while (!components.empty())
	{
		delete components.back();
		components.pop_back();
	}
	components.clear();
}

void GameObject::Update()
{
	App->renderer3D->FrustumCulling(this);

	for (int i = 0; i < childs.size(); i++)
	{
		childs[i]->Update();
	}

	for (int i = 0; i < components.size(); i++)
	{
		components[i]->Update();
	}
}

void GameObject::AddChild(GameObject* child)
{
	if (child != nullptr)
	{
		childs.push_back(child);
		child->parent = this;
	}
}

void GameObject::DeleteChilds()
{
	while (!childs.empty())
	{
		delete childs.back();
		childs.pop_back();
	}
	childs.clear();
}

void GameObject::AddComponent(Component* component)
{
	component->SetGameObject(this);
	components.push_back(component);
}

void GameObject::SetName(const char * name)
{
	this->name.assign(name);
}

void GameObject::SetParent(GameObject * parent)
{
	this->parent = parent;
}

GameObject * GameObject::GetParent()
{
	if(parent != nullptr)
	return parent;
	else return nullptr;
}

Component* GameObject::FindComponent(ComponentType type) const
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == type)
		{
			return components[i];
		}
	}

	return nullptr;
}

void GameObject::OnEditor()
{
	ImGuiTreeNodeFlags flags = 0;
	if (childs.empty())
	{
		flags |= ImGuiTreeNodeFlags_Bullet;
	}
	if (selected == true)
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}
	if (ImGui::TreeNodeEx(this, flags, this->name.c_str()))
	{
		if (ImGui::IsItemHoveredRect() && App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
		{
			App->editor->SelectGameObject(this);
		}
		for (int i = 0; i < childs.size(); i++)
		{
			childs[i]->OnEditor();
		}
		ImGui::TreePop();
	}
}

void GameObject::ShowInspector()
{
	int w, h;
	w = App->window->Window_Width;
	ImGui::SetNextWindowSize(ImVec2(300, 500));
	ImGui::SetNextWindowPos(ImVec2(w - 300, 0));

	std::string temp = name;
	temp += " Inspector";
	ImGui::Begin(temp.c_str());

	ImGui::PushItemWidth(-140);
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->OnEditor();
	}
	ImGui::End();
}

void GameObject::Move(float3 lastpos,float3 newPos)
{
	for (int i = 0; i < components.size(); i++)
	{
		if (components[i]->GetType() == Component_Mesh)
		{
			dynamic_cast<CompMesh*>(components[i])->Move(lastpos,newPos);
		}
	}
}
