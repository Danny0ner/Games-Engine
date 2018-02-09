#include "GameObject.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "CompCamera.h"
#include "CompMaterial.h"
#include "CompAnimation.h"

GameObject::GameObject(GameObject* parent): parent(parent)
{
	name = "Game Object";
	uid = App->RandomUIDGen->Int();
}
GameObject::~GameObject()
{
	if (Static == true)
	{
		App->editor->Static_Vector.erase(std::remove(App->editor->Static_Vector.begin(), App->editor->Static_Vector.end(), this), App->editor->Static_Vector.end());
	}
	else 
	{
		App->editor->Dynamic_Vector.erase(std::remove(App->editor->Dynamic_Vector.begin(), App->editor->Dynamic_Vector.end(), this), App->editor->Dynamic_Vector.end());
	}
	while (!childs.empty())
	{
		if (childs.back() != nullptr)
		{
			delete childs.back();
		}
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

void GameObject::Update(float dt)
{
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->Update(dt);
	}

	for (int i = 0; i < childs.size(); i++)
	{
		childs[i]->Update(dt);
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

void GameObject::ChangeStaticState()
{
	if (Static == true)
	{
		App->editor->Dynamic_Vector.erase(std::remove(App->editor->Dynamic_Vector.begin(), App->editor->Dynamic_Vector.end(), this), App->editor->Dynamic_Vector.end());
		App->editor->Static_Vector.push_back(this);
	}
	if (Static == false)
	{
		App->editor->Static_Vector.erase(std::remove(App->editor->Static_Vector.begin(), App->editor->Static_Vector.end(), this), App->editor->Static_Vector.end());
		App->editor->Dynamic_Vector.push_back(this);
	}
}


void GameObject::DeleteChild(GameObject * todelete)
{
	App->editor->GetRoot()->childs.erase(std::remove(App->editor->GetRoot()->childs.begin(), App->editor->GetRoot()->childs.end(), todelete), App->editor->GetRoot()->childs.end());
}

void GameObject::FindSiblingOrChildGameObjectWithName(const char * name, GameObject* &gameobj)
{
	for (int i = 0; i < parent->childs.size(); i++)
	{
		if (strcmp(name, parent->childs[i]->Getname().c_str()) == 0)
		{
			gameobj = parent->childs[i];
			return;
		}
	}
	FindChildGameObjectWithName(name, gameobj);
}

void GameObject::FindChildGameObjectWithName(const char * name, GameObject* &gameobj)
{
	for (int i = 0; i < childs.size(); i++)
	{
		if (strcmp(name, childs[i]->Getname().c_str()) == 0)
		{
			gameobj = childs[i];
			return;
		}
		childs[i]->FindChildGameObjectWithName(name, gameobj);
	}
}

void GameObject::DrawSkeletonDebug()
{
	for (int i = 0; i < childs.size(); i++)
	{
		pLine vLine(GetPosition().x* GetScale().x, GetPosition().y* GetScale().y, GetPosition().z* GetScale().z, childs[i]->GetPosition().x * childs[i]->GetScale().x, childs[i]->GetPosition().y * childs[i]->GetScale().y, childs[i]->GetPosition().z* childs[i]->GetScale().z);
		vLine.color = { 1.0f, 0.85f, 0.0f };
		vLine.Render();
		for (int x = 0; x < childs[i]->childs.size(); x++)
		{
			childs[i]->DrawSkeletonDebug();
		}
	}
}

int GameObject::GetUID()
{
	return uid;
}

float3 GameObject::GetPosition()
{
	CompTransform* trans = (CompTransform*)FindComponent(Component_Transform);
	float3 pos;
	Quat rot;
	float3 scal;
	trans->GetTransMatrix().Decompose(pos, rot, scal);
	return pos;
}

float3 GameObject::GetScale()
{
	CompTransform* trans = (CompTransform*)FindComponent(Component_Transform);
	float3 pos;
	Quat rot;
	float3 scal;
	trans->GetTransMatrix().Decompose(pos, rot, scal);
	return scal;
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
	if (ImGui::Checkbox("Static", &Static)) 
	{
		ChangeStaticState();
	}
	if (ImGui::Checkbox("Delete Childs", &deletingchilds))
	{
		DeleteChilds();
		deletingchilds = false;
	}
	if (ImGui::Button("Create Anim Component"))
	{
		CompAnimation* newanimcomp = new CompAnimation();
		this->AddComponent(newanimcomp);
	}
	ImGui::PushItemWidth(-140);
	for (int i = 0; i < components.size(); i++)
	{
		components[i]->OnEditor();
	}
	ImGui::End();
}

std::string GameObject::Getname() const
{
	return name;
}


GameObject* GameObject::FindUIDGameObject(int toFind)
{
	if (toFind == uid)
	{
		return this;
	}

	GameObject* ret = nullptr;
	for (int i = 0; i < childs.size() && ret == nullptr; i++)
	{
		ret = childs[i]->FindUIDGameObject(toFind);
	}

	return ret;

}

void GameObject::OnSerialize(Configuration& dataToSave) const
{
	if (strcmp(name.c_str(), "Root") != 0)
	{
		Configuration myConf;

		myConf.SetString("Name", name.c_str());
		myConf.SetInt("UID", uid);
		myConf.SetInt("Parent UID", parent != App->editor->GetRoot() ? parent->GetUID() : 0);

		myConf.AddArray("Components");

		for (int i = 0; i < components.size(); i++)
		{
			Configuration compConfig;
			compConfig.SetInt("Type", components[i]->GetType());
			components[i]->OnSave(compConfig);
			myConf.AddArrayEntry(compConfig);
		}
		dataToSave.AddArrayEntry(myConf);
	}

	for (int i = 0; i < childs.size(); i++)
	{
		childs[i]->OnSerialize(dataToSave);
	}
}

void GameObject::Deserialize(Configuration & dataToLoad)
{
	SetName(dataToLoad.GetString("Name"));
	uid = dataToLoad.GetInt("UID");
	int parentUID = dataToLoad.GetInt("Parent UID");
	if (parentUID == 0)
	{
		App->editor->GetRoot()->AddChild(this);
	}
	else
	{
		GameObject* parent = App->editor->GetRoot()->FindUIDGameObject(parentUID);
		if (parent != nullptr)
		{
			parent->AddChild(this);
		}
	}
	int componentSize = dataToLoad.GetArraySize("Components");

	for (int i = 0; i < componentSize; i++)
	{
		Configuration componentConfig(dataToLoad.GetArray("Components", i));
		ComponentType cType = (ComponentType)componentConfig.GetInt("Type");
		switch (cType)
		{
		case Component_Camera:
		{
			CompCamera* compCamera = new CompCamera();
			//compCamera->OnLoad(componentConfig);
			AddComponent(compCamera);
			break;
		}
		case Component_Mesh:
		{
			CompMesh* compMesh = new CompMesh();
			compMesh->OnLoad(componentConfig);
			compMesh->CreateEnclosingBox();
			AddComponent(compMesh);
			break;
		}
		case Component_Transform:
		{
			CompTransform* compTrans = new CompTransform();
			compTrans->OnLoad(componentConfig);
			AddComponent(compTrans);
			break;
		}
		case Component_Material:
		{
			CompMaterial* compMat = new CompMaterial();
			compMat->OnLoad(componentConfig);
			AddComponent(compMat);
			break;
		}
		case Component_Animation:
		{
			CompAnimation* compAnim = new CompAnimation();
			compAnim->OnLoad(componentConfig);
			AddComponent(compAnim);
			break;
		}
		default:
		{
			LOG("Error in component %i of %s, unknown type", &i, name.c_str());
			break;
		}
		}
	}
	App->editor->Dynamic_Vector.push_back(this);
}
