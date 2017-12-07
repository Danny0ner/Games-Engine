#include "compAnimation.h"
#include "Primitive.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "GameObject.h"
#include "ImGui\imgui.h"
#include "Glew\include\glew.h"
#include "MathGeo\Geometry\Triangle.h"
#include "MathGeo\Math\float4x4.h"
#include "CompTransform.h"

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
	GameObject* test = nullptr;
	if (resourceAnim != nullptr)
	{

		for (int i = 0; i < resourceAnim->bones.size(); i++)
		{
			myGO->FindSiblingOrChildGameObjectWithName(resourceAnim->bones[i]->name.c_str(), test);

			PositionKey* actualposkey = nullptr;
			for (int p = 0; p < resourceAnim->bones[i]->positionkeys.size(); p++)
			{
				if (animetime == 0)
				{
					actualposkey = resourceAnim->bones[i]->positionkeys[0];
					break;
				}
				if (resourceAnim->bones[i]->positionkeys[p]->time <= animetime)
				{
					actualposkey = resourceAnim->bones[i]->positionkeys[p];
				}
			}
			RotationKey* actualrotkey = nullptr;
			for (int p = 0; p < resourceAnim->bones[i]->rotationkeys.size(); p++)
			{
				if (animetime == 0)
				{
					actualrotkey = resourceAnim->bones[i]->rotationkeys[0];
					break;
				}
				if (resourceAnim->bones[i]->rotationkeys[p]->time <= animetime)
				{
					actualrotkey = resourceAnim->bones[i]->rotationkeys[p];
				}
			}

			CompTransform* trans = (CompTransform*)test->FindComponent(Component_Transform);
			trans->SetPosition(actualposkey->position);
			trans->SetRotation(actualrotkey->rotation);
		}
	}
	if (drawdebug)
	{
		DrawDebug();
	}
}

void CompAnimation::DrawDebug() const
{
	myGO->DrawSkeletonDebug();
}

void CompAnimation::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (resourceAnim != nullptr)
		{
			ImGui::Text("Name: %s", resourceAnim->name.c_str());
			ImGui::Text("Duration: %f", resourceAnim->duration);
			ImGui::Text("TicksPerSec: %f", resourceAnim->ticksPerSec);
			ImGui::Text("Number of Bones: %i", resourceAnim->bones.size());
			ImGui::DragFloat("AnimTime", &animetime,0.01,0,resourceAnim->duration);
		}
		if (ImGui::Checkbox("drawdebug", &drawdebug))
		{

		}
		if (ImGui::BeginMenu("Change Animation", &ChangingAnimation))
		{
			App->resources->ShowAnimationResources(this);
			ImGui::EndMenu();
		}
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

