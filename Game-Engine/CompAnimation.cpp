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
	for (std::vector<AnimationClip*>::iterator temp = animation_clips.begin(); temp != animation_clips.end(); temp++)
	{
		RELEASE((*temp));
	}
}


void CompAnimation::AnimationMoves()
{
	/*if (animation_clips.size() >= 3)
	{
		if (ActualClip == animation_clips[2] && ActualClip->finished == true)
		{
			LastClip = ActualClip;
			ActualClip = animation_clips[0];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->start_frame_time;
		}

		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
		{
			LastClip = ActualClip;
			ActualClip = animation_clips[1];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->start_frame_time;
		}
		if (App->input->GetKey(SDL_SCANCODE_2) == KEY_UP)
		{
			LastClip = ActualClip;
			ActualClip = animation_clips[0];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->start_frame_time;
		}
		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
		{
			LastClip = ActualClip;
			ActualClip = animation_clips[2];
			LastClip->ActuallyRunning = false;
			ActualClip->ActuallyRunning = true;
			AnimState = A_BLENDING;
			nextanimetime = ActualClip->start_frame_time;
			ActualClip->finished = false;
		}
	}*/
}



void CompAnimation::Update(float dt)
{
	std::vector<AnimationClip*> clips_playing;
	for (std::vector<AnimationClip*>::const_iterator it = animation_clips.begin(); it != animation_clips.end(); ++it)
		if ((*it)->state == AnimationState::A_PLAY)
		{
			(*it)->time += dt;
			if ((*it)->time > (*it)->end_frame_time)
			{
				if ((*it)->loop == true)
				{
					(*it)->RestartAnimationClip();

					clips_playing.push_back(*it);
				}
				else
					(*it)->state = AnimationState::A_STOP;
			}			
			else
				clips_playing.push_back(*it);

		}

	for (std::vector<std::pair<GameObject*,const Bone*>>::iterator it = bone_update_vector.begin(); it != bone_update_vector.end(); ++it)
		it->second->UpdateBone(it->first, clips_playing);

	/*if (App->GetGameStatus() == GameStatus::PLAY)
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
			if (!bonesplaceds)
			{
				for (int i = 0; i < resource->bones.size(); i++)
				{
					myGO->FindSiblingOrChildGameObjectWithName(resource->bones[i]->name.c_str(), test);
					bonesGOs[resource->bones[i]->name.c_str()] = test;
				}
				bonesplaceds = true;
			}
			if (ActualClip != nullptr)
			{
				animetime += dt;
				animetime += TicksPerSecond / resource->duration;

				if (animetime >= ActualClip->end_frame_time) animetime = ActualClip->end_frame_time;

				for (int i = 0; i < resource->bones.size(); i++)
				{
					test = bonesGOs[resource->bones[i]->name.c_str()];

					for (int p = 0, a = 1; p < resource->bones[i]->positionkeys.size(); p++, a++)
					{
						SetActualPositionKey(actualposkey, nextposkey, resource->bones[i], p, animetime);
					}
					for (int p = 0, a = 1; p < resource->bones[i]->rotationkeys.size(); p++, a++)
					{
						SetActualRotationKey(actualrotkey, nextrotkey, resource->bones[i], p, animetime);
					}

					SetBonePosition(test, actualposkey, nextposkey);
					SetBoneRotation(test, actualrotkey, nextrotkey);
				}

				if (animetime >= ActualClip->end_frame_time)
				{
					if (ActualClip->Loop == true)
					{
						animetime = ActualClip->start_frame_time;
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
			animetime += TicksPerSecond / resource->duration;

			nextanimetime += dt;
			nextanimetime += TicksPerSecond / resource->duration;

			for (int i = 0; i < resource->bones.size(); i++)
			{
				myGO->FindSiblingOrChildGameObjectWithName(resource->bones[i]->name.c_str(), test);

				for (int p = 0; p < resource->bones[i]->positionkeys.size(); p++)
				{
					SetActualPositionKey(actualposkey, nextposkey, resource->bones[i], p, animetime);
					SetActualPositionKey(BLactualposkey, BLnextposkey, resource->bones[i], p, nextanimetime);

				}
				for (int p = 0; p < resource->bones[i]->rotationkeys.size(); p++)
				{
					SetActualRotationKey(actualrotkey, nextrotkey, resource->bones[i], p, animetime);
					SetActualRotationKey(BLactualrotkey, BLnextrotkey, resource->bones[i], p, nextanimetime);
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

			if (animetime >= LastClip->end_frame_time)
			{
				animetime = LastClip->start_frame_time;
			}
			if (nextanimetime >= ActualClip->end_frame_time)
			{
				nextanimetime = ActualClip->start_frame_time;
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
	*/
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
		if (resource != nullptr)
		{
			ImGui::Text("Name: %s", resource->name.c_str());
			ImGui::Text("Duration: %f", resource->duration);
			ImGui::Text("TicksPerSec: %f", resource->ticksPerSec);
			ImGui::Text("Number of Bones: %i", resource->bones.size());
			ImGui::DragFloat("AnimTime", &animetime, 0.01, 0, resource->duration);
			ImGui::DragFloat("TicksPerSec", &TicksPerSecond, 0.01, 0, 60);
			ImGui::Checkbox("Interpolation", &Interpolation);
			ImGui::DragFloat("Blending Time", &blendingtime, 0.10, 0.1, 10);
			if(ImGui::Button("Create Animation Clip",ImVec2(125, 25)))
			{
				AnimationClip* tempanimclip = new AnimationClip();
				tempanimclip->name += std::to_string(animation_clips.size()).c_str();
				tempanimclip->end_frame_time = resource->duration;
				if (animation_clips.size() == 0)
				{
					ActualClip = tempanimclip;
					tempanimclip->ActuallyRunning = true;
				}
				animation_clips.push_back(tempanimclip);
			}
			if (animation_clips.size() > 0)
			{
				ImGui::Separator();
				for (int i = 0; i < animation_clips.size(); i++)
				{
					ImGui::Text(animation_clips[i]->name.c_str());
				
					ImGui::PushID(i);
					if (ImGui::Checkbox("Loop", &animation_clips[i]->loop)) {}
					if (ImGui::Checkbox("Actually Running", &animation_clips[i]->ActuallyRunning)) 
					{
						if (ActualClip == animation_clips[i])
						{
							ActualClip = nullptr;
							
						}
						else
						{
							for (uint j = 0; j < animation_clips.size(); j++)
							{
								if (j != i)
								{
									animation_clips[j]->ActuallyRunning = false;
								}
							}
							if (ActualClip != nullptr)
							{
								LastClip = ActualClip;
								ActualClip = animation_clips[i];
								nextanimetime = ActualClip->start_frame_time;
							}
							if (ActualClip == nullptr)
							{
								ActualClip = animation_clips[i];
								AnimState = A_PLAY;
								animetime = animation_clips[i]->start_frame_time;
							}
						}
					}
					ImGui::DragFloat("Start Frame", &animation_clips[i]->start_frame_time, 0.01, 0, resource->duration);
					ImGui::DragFloat("End Frame", &animation_clips[i]->end_frame_time, 0.01, 0, 60);
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
	if (resource != nullptr)
	{
		data.SetString("ResourceAnim Name", resource->GetFile());
	}
	else
	{
		data.SetString("ResourceAnim Name", "noresource");
	}
	data.AddArray("Animation Clips");

	for (std::vector<AnimationClip*>::const_iterator temp = animation_clips.begin(); temp != animation_clips.end(); temp++)
	{
		Configuration AnimClip;

		AnimClip.SetString("Name", (*temp)->name.c_str());
		AnimClip.SetFloat("StartFrame", (*temp)->start_frame_time);
		AnimClip.SetFloat("EndFrame", (*temp)->end_frame_time);
		AnimClip.SetBool("Loop", (*temp)->loop);
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
		tmpanimclip->start_frame_time = loadAnimClip.GetFloat("StartFrame");
		tmpanimclip->end_frame_time = loadAnimClip.GetFloat("EndFrame");
		tmpanimclip->loop = loadAnimClip.GetBool("Loop");
		tmpanimclip->ActuallyRunning = loadAnimClip.GetBool("Actually Running");
		if (tmpanimclip->ActuallyRunning == true)
		{
			ActualClip = tmpanimclip;
		}
		animation_clips.push_back(tmpanimclip);
	}
}

void CompAnimation::AddResourceByName(std::string filename)
{
	resource = (ResourceAnimation*)App->resources->GetResourceByName(filename.c_str());
	if (resource != nullptr)
		resource->LoadToComponent();


	GameObject* test;
	for (int i = 0; i < resource->bones.size(); i++)
	{
		myGO->FindSiblingOrChildGameObjectWithName(resource->bones[i]->name.c_str(), test);
		bone_update_vector.push_back(std::make_pair(test, resource->bones[i]));
	}
	bonesplaceds = true;


	TicksPerSecond = 0.0f;
}

void CompAnimation::AddResource(int uid)
{
	resource = (ResourceAnimation*)App->resources->Get(uid);
	if (resource != nullptr)
		resource->LoadToComponent();

	GameObject* test;
	for (int i = 0; i < resource->bones.size(); i++)
	{
		myGO->FindSiblingOrChildGameObjectWithName(resource->bones[i]->name.c_str(), test);
		bone_update_vector.push_back(std::make_pair(test, resource->bones[i]));
	}
	bonesplaceds = true;

	TicksPerSecond = resource->ticksPerSec;
	if (resource->duration / TicksPerSecond >= 1)
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
	if (resource != nullptr)
		return resource;
	else
		return nullptr;

}

void AnimationClip::RestartAnimationClip()
{
	time = start_frame_time;
}
