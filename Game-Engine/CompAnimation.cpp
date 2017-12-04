#include "compAnimation.h"
#include "Primitive.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "GameObject.h"
#include "ImGui\imgui.h"
#include "Glew\include\glew.h"
#include "MathGeo\Geometry\Triangle.h"
#include "MathGeo\Math\float4x4.h"

#include <vector>
CompAnimation::CompAnimation() : Component(Component_Animation)
{
	name = "Animation";
}

CompAnimation::~CompAnimation()
{

}

void CompAnimation::Update()
{
}

void CompAnimation::DrawDebug() const
{
	for (int i = 0; i < myGO->childs.size(); i++)
	{
		pLine vLine(myGO->GetPosition().x, myGO->GetPosition().y, myGO->GetPosition().z, myGO->childs[i]->GetPosition().x, myGO->childs[i]->GetPosition().y, myGO->childs[i]->GetPosition().z);
		vLine.color = { 1.0f, 0.85f, 0.0f };
		vLine.Render();
			for (int x = 0; x < myGO->childs[i]->childs.size(); x++)
			{
				pLine VlineChild(myGO->childs[i]->GetPosition().x, myGO->childs[i]->GetPosition().y, myGO->childs[i]->GetPosition().z, myGO->childs[i]->childs[x]->GetPosition().x, myGO->childs[i]->childs[x]->GetPosition().y, myGO->childs[i]->childs[x]->GetPosition().z);
				VlineChild.color = { 1.0f, 0.85f, 0.0f };
				VlineChild.Render();
			}
	}
}

void CompAnimation::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TreePop();
	}
}

void CompAnimation::OnSave(Configuration & data) const
{
	/*if (resourceMesh != nullptr)
	{
		data.SetString("ResourceMesh Name", resourceMesh->GetFile());
	}
	else
	{
		data.SetString("ResourceMesh Name", "noresource");
	}*/

}

void CompAnimation::OnLoad(Configuration & data)
{
	//AddResourceByName(data.GetString("ResourceMesh Name"));
}

void CompAnimation::AddResourceByName(std::string filename)
{
	resourceAnim = (ResourceAnimation*)App->resources->GetResourceByName(filename.c_str());
	if (resourceAnim != nullptr)
		resourceAnim->LoadToComponent();
}

void CompAnimation::AddResource(int uid)
{
	resourceAnim = (ResourceAnimation*)App->resources->Get(uid);
	if (resourceAnim != nullptr)
		resourceAnim->LoadToComponent();
}

ResourceAnimation * CompAnimation::GetResourceAnim()
{
	if (resourceAnim != nullptr)
		return resourceAnim;
	else
		return nullptr;

}

