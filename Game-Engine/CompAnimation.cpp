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
	PositionKey* actualposkey = nullptr;
	PositionKey* nextposkey = nullptr;
	RotationKey* actualrotkey = nullptr;
	RotationKey* nextrotkey = nullptr;

	GameObject* test = nullptr;

	switch (AnimState)
	{
	case A_PLAY:

		if (ActualClip != nullptr)
		{
			animetime += dt;
			animetime += TicksPerSecond / resourceAnim->duration;
			if (animetime >= ActualClip->EndFrameTime) animetime = ActualClip->EndFrameTime;
			for (int i = 0; i < resourceAnim->bones.size(); i++)
			{
				myGO->FindSiblingOrChildGameObjectWithName(resourceAnim->bones[i]->name.c_str(), test);

				for (int p = 0, a = 1; p < resourceAnim->bones[i]->positionkeys.size(); p++, a++)
				{
					SetActualPositionKey(actualposkey, nextposkey, resourceAnim->bones[i], p);
					
				}
				for (int p = 0, a = 1; p < resourceAnim->bones[i]->rotationkeys.size(); p++, a++)
				{
					SetActualRotationKey(actualrotkey, nextrotkey, resourceAnim->bones[i],p);
				}
				SetBone(test, actualposkey, nextposkey, actualrotkey, nextrotkey);
			}
			if (ActualClip->Loop == true)
			{
				if (animetime >= ActualClip->EndFrameTime) animetime = ActualClip->StartFrameTime;
			}
		}
		

		break;

	case A_STOP:

		animetime = 0.0f;

		break;

	case A_BLENDING:


		break;
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
							for (uint j = 0; j < animationclips.size(); j++)
							{
								if (j != i)
								{
									animationclips[j]->ActuallyRunning = false;

								}
							}
							LastClip = ActualClip;
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

void CompAnimation::SetBone(GameObject * Bone, PositionKey * ActualPos, PositionKey * NextPos, RotationKey * ActualRot, RotationKey * NextRot)
{
	if (Bone != nullptr)
	{
		float time = 0;
		CompTransform* trans = (CompTransform*)Bone->FindComponent(Component_Transform);
		if (Interpolation == true)
		{
			if (ActualPos != nullptr && NextPos != nullptr)
			{
				if (ActualPos == NextPos)
				{
					trans->SetPosition(ActualPos->position);
				}
				else
				{
					time = (animetime - ActualPos->time) / (NextPos->time - ActualPos->time);
					float3 position = float3::Lerp(ActualPos->position, NextPos->position, time);
					trans->SetPosition(position);
				}
			}
			if (ActualRot != nullptr && NextRot != nullptr)
			{
				if (ActualRot == NextRot)
				{
					trans->SetRotation(ActualRot->rotation);
				}
				else
				{

					Quat ActualQuat = Quat(ActualRot->rotation.x, ActualRot->rotation.y, ActualRot->rotation.z, ActualRot->rotation.w);
					Quat NextQuat = Quat(NextRot->rotation.x, NextRot->rotation.y, NextRot->rotation.z, NextRot->rotation.w);

					time = (animetime - ActualRot->time) / (NextRot->time - ActualRot->time);
					Quat rotation = ActualQuat.Slerp(NextQuat, time);
					trans->SetRotation(rotation);
				}
			}
		}
		else
		{
			if (ActualPos != nullptr)
			{
				trans->SetPosition(ActualPos->position);
			}

			if (ActualRot != nullptr)
			{
				trans->SetRotation(ActualRot->rotation);
			}
		}
	}


}

void CompAnimation::SetActualRotationKey(RotationKey * &Actual, RotationKey * &Next, Bone* bone, int p)
{
	if (animetime == 0)
	{
		Actual = bone->rotationkeys[0];
		if (bone->rotationkeys.size() > 1)
		{
			Next = bone->rotationkeys[1];
		}
		else
		{
			Next = Actual;
		}
		
	}
	if (bone->rotationkeys[p]->time < animetime)
	{
		Actual = bone->rotationkeys[p];
		if (bone->rotationkeys.size() > 1 && p+1 < bone->rotationkeys.size())
		{
			Next = bone->rotationkeys[p+1];
		}
		else
		{
			Next = bone->rotationkeys[p];
		}
	}

}

void CompAnimation::SetActualPositionKey(PositionKey * &Actual, PositionKey * &Next,Bone* bone, int p)
{
	if (animetime == 0)
	{
		Actual = bone->positionkeys[0];
		if (bone->positionkeys.size() > 1)
		{
			Next = bone->positionkeys[1];
		}
		else
		{
			Next = Actual;
		}
		
	}
	if (bone->positionkeys[p]->time < animetime)
	{
		Actual = bone->positionkeys[p];
		if (bone->positionkeys.size() > 1 && p+1 < bone->positionkeys.size())
		{
			Next = bone->positionkeys[p+1];
		}
		else
		{
			Next = bone->positionkeys[p];
		}
	}
}

ResourceAnimation * CompAnimation::GetResourceAnim()
{
	if (resourceAnim != nullptr)
		return resourceAnim;
	else
		return nullptr;

}

