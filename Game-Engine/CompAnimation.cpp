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
	for (std::vector<AnimationClip*>::iterator temp = animationclips.begin(); temp != animationclips.end(); temp++)
	{
		RELEASE((*temp));
	}
}


void CompAnimation::AnimationMoves()
{
	if (animationclips.size() >= 3)
	{
		if (ActualClip == animationclips[2] && ActualClip->finished == true)
		{
			LastClip = ActualClip;
			ActualClip = animationclips[0];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->StartFrameTime;
		}

		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			LastClip = ActualClip;
			ActualClip = animationclips[1];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->StartFrameTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
		{
			LastClip = ActualClip;
			ActualClip = animationclips[0];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->StartFrameTime;
		}
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			LastClip = ActualClip;
			ActualClip = animationclips[2];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->StartFrameTime;
			ActualClip->finished = false;
		}
	}
}



void CompAnimation::Update(float dt)
{
	if (App->GetGameStatus() == GameStatus::PLAY)
	{
		AnimationMoves();

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
						SetActualPositionKey(actualposkey, nextposkey, resourceAnim->bones[i], p, animetime);

					}
					for (int p = 0, a = 1; p < resourceAnim->bones[i]->rotationkeys.size(); p++, a++)
					{
						SetActualRotationKey(actualrotkey, nextrotkey, resourceAnim->bones[i], p, animetime);
					}

					SetBonePosition(test, actualposkey, nextposkey);
					SetBoneRotation(test, actualrotkey, nextrotkey);
				}

				if (animetime >= ActualClip->EndFrameTime)
				{
					if (ActualClip->Loop == true)
					{
						animetime = ActualClip->StartFrameTime;
						ActualClip->finished = true;
					}
					else
					{
						ActualClip->finished = true;
					}
				}
				else
				{
					ActualClip->finished = false;
				}
			}



			break;

		case A_STOP:

			animetime = 0.0f;

			break;

		case A_BLENDING:

			PositionKey* BLactualposkey = nullptr;
			PositionKey* BLnextposkey = nullptr;

			RotationKey* BLactualrotkey = nullptr;
			RotationKey* BLnextrotkey = nullptr;

			blendtime += dt;
			animetime += dt;
			animetime += TicksPerSecond / resourceAnim->duration;

			nextanimetime += dt;
			nextanimetime += TicksPerSecond / resourceAnim->duration;

			for (int i = 0; i < resourceAnim->bones.size(); i++)
			{
				myGO->FindSiblingOrChildGameObjectWithName(resourceAnim->bones[i]->name.c_str(), test);

				for (int p = 0; p < resourceAnim->bones[i]->positionkeys.size(); p++)
				{
					SetActualPositionKey(actualposkey, nextposkey, resourceAnim->bones[i], p, animetime);
					SetActualPositionKey(BLactualposkey, BLnextposkey, resourceAnim->bones[i], p, nextanimetime);

				}
				for (int p = 0; p < resourceAnim->bones[i]->rotationkeys.size(); p++)
				{
					SetActualRotationKey(actualrotkey, nextrotkey, resourceAnim->bones[i], p, animetime);
					SetActualRotationKey(BLactualrotkey, BLnextrotkey, resourceAnim->bones[i], p, nextanimetime);
				}

				float3 actualPosition = GetBonePosition(test, actualposkey, nextposkey, animetime);
				Quat actualRotation = GetBoneRotation(test, actualrotkey, nextrotkey, animetime);

				float3 nextPosition = GetBonePosition(test, BLactualposkey, BLnextposkey, nextanimetime);
				Quat nextRotation = GetBoneRotation(test, BLactualrotkey, BLnextrotkey, nextanimetime);

				float3 position = float3::Lerp(actualPosition, nextPosition, (blendtime / blendingtime));
				Quat rotation = Quat::Slerp(actualRotation, nextRotation, (blendtime / blendingtime));

				CompTransform* BoneTransform = (CompTransform*)(test->FindComponent(Component_Transform));
				BoneTransform->SetPosition(position);
				BoneTransform->SetRotation(rotation);

			}

			if (animetime >= LastClip->EndFrameTime)
			{
				animetime = LastClip->StartFrameTime;
			}
			if (nextanimetime >= ActualClip->EndFrameTime)
			{
				nextanimetime = ActualClip->StartFrameTime;
			}

			if (blendtime >= blendingtime)
			{
				LastClip = nullptr;
				blendtime = 0.0f;
				AnimState = A_PLAY;
				animetime = nextanimetime;
			}


			break;
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
			ImGui::DragFloat("Blending Time", &blendingtime, 0.10, 0.1, 10);
			if(ImGui::Button("Create Animation Clip",ImVec2(125, 25)))
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
							if (ActualClip != nullptr)
							{
								LastClip = ActualClip;
								ActualClip = animationclips[i];
								AnimState = A_BLENDING;
								nextanimetime = ActualClip->StartFrameTime;
							}
							if (ActualClip == nullptr)
							{
								ActualClip = animationclips[i];
								AnimState = A_PLAY;
								animetime = animationclips[i]->StartFrameTime;
							}
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
	if (resourceAnim != nullptr)
	{
		data.SetString("ResourceAnim Name", resourceAnim->GetFile());
	}
	else
	{
		data.SetString("ResourceAnim Name", "noresource");
	}
	data.AddArray("Animation Clips");

	for (std::vector<AnimationClip*>::const_iterator temp = animationclips.begin(); temp != animationclips.end(); temp++)
	{
		Configuration AnimClip;

		AnimClip.SetString("Name", (*temp)->name.c_str());
		AnimClip.SetFloat("StartFrame", (*temp)->StartFrameTime);
		AnimClip.SetFloat("EndFrame", (*temp)->EndFrameTime);
		AnimClip.SetBool("Loop", (*temp)->Loop);
		AnimClip.SetBool("Actually Running", (*temp)->ActuallyRunning);
		data.AddArrayEntry(AnimClip);
	}

}

void CompAnimation::OnLoad(Configuration & data)
{
	AddResourceByName(data.GetString("ResourceAnim Name"));

	for (int i = 0; i < data.GetArraySize("Animation Clips"); i++)
	{
		AnimationClip* tmpanimclip = new AnimationClip();
		Configuration loadAnimClip = data.GetArray("Animation Clips", i);

		tmpanimclip->name = loadAnimClip.GetString("Name");
		tmpanimclip->StartFrameTime = loadAnimClip.GetFloat("StartFrame");
		tmpanimclip->EndFrameTime = loadAnimClip.GetFloat("EndFrame");
		tmpanimclip->Loop = loadAnimClip.GetBool("Loop");
		tmpanimclip->ActuallyRunning = loadAnimClip.GetBool("Actually Running");
		if (tmpanimclip->ActuallyRunning == true)
		{
			ActualClip = tmpanimclip;
		}
		animationclips.push_back(tmpanimclip);
	}
}

void CompAnimation::AddResourceByName(std::string filename)
{
	resourceAnim = (ResourceAnimation*)App->resources->GetResourceByName(filename.c_str());
	if (resourceAnim != nullptr)
		resourceAnim->LoadToComponent();

	TicksPerSecond = 0.0f;
}

void CompAnimation::AddResource(int uid)
{
	resourceAnim = (ResourceAnimation*)App->resources->Get(uid);
	if (resourceAnim != nullptr)
		resourceAnim->LoadToComponent();

	TicksPerSecond = resourceAnim->ticksPerSec;
	if (resourceAnim->duration / TicksPerSecond >= 1)
		TicksPerSecond = 0;
}


void CompAnimation::SetBlendingBone(GameObject * Bone, PositionKey * ActualPos, PositionKey * NextPos, RotationKey * ActualRot, RotationKey * NextRot, float time)
{
	time = time / blendingtime;
	if (Bone != nullptr)
	{
		CompTransform* trans = (CompTransform*)Bone->FindComponent(Component_Transform);

		if (ActualPos != nullptr && NextPos != nullptr)
		{
			if (ActualPos == NextPos)
			{
				trans->SetPosition(ActualPos->position);
			}
			else
			{
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
				Quat rotation = ActualQuat.Slerp(NextQuat, time);
				trans->SetRotation(rotation);
			}
		}
	}
}

void CompAnimation::SetActualRotationKey(RotationKey * &Actual, RotationKey * &Next, Bone* bone, int p, float time)
{
	if (time == 0)
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
	if (bone->rotationkeys[p]->time < time)
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

void CompAnimation::SetActualPositionKey(PositionKey * &Actual, PositionKey * &Next,Bone* bone, int p,float time)
{
	if (time == 0)
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
	if (bone->positionkeys[p]->time < time)
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

void CompAnimation::SetBonePosition(GameObject * Bone, PositionKey * ActualPos, PositionKey * NextPos)
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
		}
		else
		{
			if (ActualPos != nullptr)
			{
				trans->SetPosition(ActualPos->position);
			}
		}
	}

}

void CompAnimation::SetBoneRotation(GameObject * Bone, RotationKey * ActualRot, RotationKey * NextRot)
{
	if (Bone != nullptr)
	{
		float time = 0;
		CompTransform* trans = (CompTransform*)Bone->FindComponent(Component_Transform);
		if (Interpolation == true)
		{
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

			if (ActualRot != nullptr)
			{
				trans->SetRotation(ActualRot->rotation);
			}
		}
	}

}

float3 CompAnimation::GetBonePosition(GameObject * Bone, PositionKey * ActualPos, PositionKey * NextPos, float timeanim)
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
					return ActualPos->position;
				}
				else
				{
					time = (timeanim - ActualPos->time) / (NextPos->time - ActualPos->time);
					float3 position = float3::Lerp(ActualPos->position, NextPos->position, time);
					return position;
				}
			}
		}
		else
		{
			if (ActualPos != nullptr)
			{
				return ActualPos->position;
			}
		}
	}
}


Quat CompAnimation::GetBoneRotation(GameObject * Bone, RotationKey * ActualRot, RotationKey * NextRot, float timeanim)
{
	if (Bone != nullptr)
	{
		float time = 0;
		CompTransform* trans = (CompTransform*)Bone->FindComponent(Component_Transform);
		if (Interpolation == true)
		{
			if (ActualRot != nullptr && NextRot != nullptr)
			{
				if (ActualRot == NextRot)
				{
					Quat finalQuat = Quat(ActualRot->rotation.x, ActualRot->rotation.y, ActualRot->rotation.z, ActualRot->rotation.w);
					return  finalQuat;
				}
				else
				{

					Quat ActualQuat = Quat(ActualRot->rotation.x, ActualRot->rotation.y, ActualRot->rotation.z, ActualRot->rotation.w);
					Quat NextQuat = Quat(NextRot->rotation.x, NextRot->rotation.y, NextRot->rotation.z, NextRot->rotation.w);

					time = (timeanim - ActualRot->time) / (NextRot->time - ActualRot->time);
					Quat rotation = ActualQuat.Slerp(NextQuat, time);
					return rotation;
					
				}
			}
		}
		else
		{

			if (ActualRot != nullptr)
			{
				Quat finalQuat = Quat(ActualRot->rotation.x, ActualRot->rotation.y, ActualRot->rotation.z, ActualRot->rotation.w);
				return  finalQuat;
			}
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

