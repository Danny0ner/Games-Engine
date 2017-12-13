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
#include "mmgr\mmgr.h"

#include <vector>
CompAnimation::CompAnimation() : Component(Component_Animation)
{
	name = "Animation";
}

CompAnimation::~CompAnimation()
{

}

void CompAnimation::Update(float dt)
{
	GameObject* test = nullptr;
	if (ActualClip != nullptr)
	{
		animetime += dt;
		animetime += TicksPerSecond / resourceAnim->duration;
		if (animetime >= ActualClip->EndFrameTime) animetime = ActualClip->EndFrameTime;
		for (int i = 0; i < resourceAnim->bones.size(); i++)
		{
			myGO->FindSiblingOrChildGameObjectWithName(resourceAnim->bones[i]->name.c_str(), test);
			PositionKey* actualposkey = nullptr;
			PositionKey* nextposkey = nullptr;
			for (int p = 0, a = 1; p < resourceAnim->bones[i]->positionkeys.size(); p++, a++)
			{
				if (animetime == 0)
				{
					actualposkey = resourceAnim->bones[i]->positionkeys[0];
					if (resourceAnim->bones[i]->positionkeys.size() > 1)
					{
						nextposkey = resourceAnim->bones[i]->positionkeys[1];
					}
					else
					{
						nextposkey = actualposkey;
					}
					break;
				}
				if (resourceAnim->bones[i]->positionkeys[p]->time < animetime)
				{
					actualposkey = resourceAnim->bones[i]->positionkeys[p];
					if (resourceAnim->bones[i]->positionkeys.size() > 1 && a < resourceAnim->bones[i]->positionkeys.size())
					{
						nextposkey = resourceAnim->bones[i]->positionkeys[a];
					}
					else
					{
						nextposkey = resourceAnim->bones[i]->positionkeys[p];
					}
				}
			}
			RotationKey* actualrotkey = nullptr;
			RotationKey* nextrotkey = nullptr;
			for (int p = 0, a = 1; p < resourceAnim->bones[i]->rotationkeys.size(); p++, a++)
			{
				if (animetime == 0)
				{
					actualrotkey = resourceAnim->bones[i]->rotationkeys[0];
					if (resourceAnim->bones[i]->rotationkeys.size() > 1)
					{
						nextrotkey = resourceAnim->bones[i]->rotationkeys[1];
					}
					else
					{
						nextrotkey = actualrotkey;
					}
					break;
				}
				if (resourceAnim->bones[i]->rotationkeys[p]->time < animetime)
				{
					actualrotkey = resourceAnim->bones[i]->rotationkeys[p];
					if (resourceAnim->bones[i]->rotationkeys.size() > 1 && a < resourceAnim->bones[i]->rotationkeys.size())
					{
						nextrotkey = resourceAnim->bones[i]->rotationkeys[a];
					}
					else
					{
						nextrotkey = actualrotkey;
					}
				}
			}
			if (test != nullptr)
			{
				float time = 0;
				CompTransform* trans = (CompTransform*)test->FindComponent(Component_Transform);
				if (Interpolation == true)
				{
					if (actualposkey != nullptr && nextposkey != nullptr)
					{
						if (actualposkey == nextposkey)
						{
							trans->SetPosition(actualposkey->position);
						}
						else
						{
							time = (animetime - actualposkey->time) / (nextposkey->time - actualposkey->time);
							float3 position = float3::Lerp(actualposkey->position, nextposkey->position, time);
							trans->SetPosition(position);
						}
					}
					if (actualrotkey != nullptr && nextrotkey != nullptr)
					{
						if (actualrotkey == nextrotkey)
						{
							trans->SetRotation(actualrotkey->rotation);
						}
						else
						{
							Quat ActualQuat = Quat(actualrotkey->rotation.x, actualrotkey->rotation.y, actualrotkey->rotation.z, actualrotkey->rotation.w);
							Quat NextQuat = Quat(nextrotkey->rotation.x, nextrotkey->rotation.y, nextrotkey->rotation.z, nextrotkey->rotation.w);

							time = (animetime - actualrotkey->time) / (nextrotkey->time - actualrotkey->time);
							Quat rotation = ActualQuat.Slerp(NextQuat, time);
							trans->SetRotation(rotation);
						}
					}
				}
				else
				{
					if (actualposkey != nullptr)
					{
						trans->SetPosition(actualposkey->position);
					}

					if (actualrotkey != nullptr)
					{
						trans->SetRotation(actualrotkey->rotation);
					}
				}
			}
			if (ActualClip->Loop == true)
			{
				if (animetime >= ActualClip->EndFrameTime) animetime = ActualClip->StartFrameTime;
			}
		}
	}
	if (drawdebug)	DrawDebug();
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
			ImGui::DragFloat("AnimTime", &animetime, 0.01, 0, resourceAnim->duration);
			ImGui::DragFloat("TicksPerSec", &TicksPerSecond, 0.01, 0, 60);
			ImGui::Checkbox("Interpolation", &Interpolation);
			if(ImGui::Button("Create Animation Clip",ImVec2(40, 25)))
			{
				AnimationClip* tempanimclip = new AnimationClip();
				tempanimclip->name += std::to_string(animationclips.size()).c_str();
				tempanimclip->EndFrameTime = resourceAnim->duration;
				if (animationclips.size() == 0)
				{
					ActualClip = tempanimclip;
					tempanimclip->ActuallyRunning = true;
				}
				animationclips.push_back(tempanimclip);
			}
			if (animationclips.size() > 0)
			{
				ImGui::Separator();
				for (int i = 0; i < animationclips.size(); i++)
				{
					ImGui::Text(animationclips[i]->name.c_str());
				
					ImGui::PushID(i);
					if (ImGui::Checkbox("Loop", &animationclips[i]->Loop)) {}
					if (ImGui::Checkbox("Actually Running", &animationclips[i]->ActuallyRunning)) 
					{
						if (ActualClip == animationclips[i])
						{
							ActualClip = nullptr;
						}
						else
						{
							ActualClip = animationclips[i];
						}
					}
					ImGui::DragFloat("Start Frame", &animationclips[i]->StartFrameTime, 0.01, 0, resourceAnim->duration);
					ImGui::DragFloat("End Frame", &animationclips[i]->EndFrameTime, 0.01, 0, 60);
					ImGui::PopID();
					ImGui::Separator();
				}
			}

		}
		if (ImGui::Checkbox("drawdebug", &drawdebug)){}
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

	TicksPerSecond = resourceAnim->ticksPerSec;
}

void CompAnimation::AddResource(int uid)
{
	resourceAnim = (ResourceAnimation*)App->resources->Get(uid);
	if (resourceAnim != nullptr)
		resourceAnim->LoadToComponent();

	TicksPerSecond = resourceAnim->ticksPerSec;
	if (resourceAnim->duration / TicksPerSecond <= 1)
		TicksPerSecond = 0;
}

ResourceAnimation * CompAnimation::GetResourceAnim()
{
	if (resourceAnim != nullptr)
		return resourceAnim;
	else
		return nullptr;

}

