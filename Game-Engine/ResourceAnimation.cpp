#include "ResourceAnimation.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
#include "CompTransform.h"

ResourceAnimation::ResourceAnimation(int UID) : Resource(UID, Resource_Animation)
{
}

ResourceAnimation::~ResourceAnimation()
{
	for (std::vector<Bone*>::iterator temp = bones.begin(); temp != bones.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void ResourceAnimation::ReimportResource()
{
	//App->geometryloader->UnloadTexture(textureID);
}

bool ResourceAnimation::LoadInMemory()
{
	App->geometryloader->LoadAnimation(exportedFile.c_str(), this);
	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	for (std::vector<Bone*>::iterator temp = bones.begin(); temp != bones.end(); temp++)
	{
		RELEASE((*temp));
	}
	return true;
}



Bone::~Bone()
{
	for (std::vector<PositionKey*>::iterator temp = positionkeys.begin(); temp != positionkeys.end(); temp++)
	{
		RELEASE((*temp));
	}
	for (std::vector<RotationKey*>::iterator temp = rotationkeys.begin(); temp != rotationkeys.end(); temp++)
	{
		RELEASE((*temp));
	}
	for (std::vector<ScaleKey*>::iterator temp = scalekeys.begin(); temp != scalekeys.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void Bone::UpdateBone(GameObject* bone, std::vector<AnimationClip*>& clip_vec) const
{
	float3 pos;
	Quat rot;
	float3 scale;

	for (std::vector<AnimationClip*>::const_iterator it = clip_vec.begin(); it != clip_vec.end(); ++it)
	{
		pos = GetPosition(*it);
		rot = GetRotation(*it);
		scale = GetScale(*it);
	}

	//TODO Blending
	
	CompTransform* trans = (CompTransform*)bone->FindComponent(Component_Transform);
	trans->SetPosition(pos);
	trans->SetRotation(rot);
	trans->SetScale(scale);
}

float3 Bone::GetPosition(AnimationClip* clip_vec) const
{
	//for(std::vector<PositionKey*>::iterator )
	if (positionkeys.size() > 1)
	{
		float3 actual_pos;
		float3 next_pos;
		float actual_time;
		float next_time;

		for (std::vector<PositionKey*>::const_iterator it = positionkeys.begin(); it != positionkeys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == positionkeys.end() - 1)
			{
				return (*positionkeys.end() - 1)->position;
			}
			else
			{
				actual_pos = (*it)->position;
				actual_time = (*it)->time;
				next_pos = (*(it+1))->position;
				next_time = (*(it+1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 
		
		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Lerp(next_pos, weight);

	}
	return positionkeys[0]->position;
}

Quat Bone::GetRotation(AnimationClip* clip_vec) const
{
	if (rotationkeys.size() > 1)
	{
		Quat actual_pos;
		Quat next_pos;
		float actual_time;
		float next_time;

		for (std::vector<RotationKey*>::const_iterator it = rotationkeys.begin(); it != rotationkeys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == rotationkeys.end() - 1)
			{
				return (*rotationkeys.end() - 1)->rotation;
			}
			else
			{
				actual_pos = (*it)->rotation;
				actual_time = (*it)->time;
				next_pos = (*(it + 1))->rotation;
				next_time = (*(it + 1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Slerp(next_pos, weight);
	}
	return rotationkeys[0]->rotation;
}

float3 Bone::GetScale(AnimationClip* clip_vec) const
{
	if (scalekeys.size() > 1)
	{
		float3 actual_pos;
		float3 next_pos;
		float actual_time;
		float next_time;

		for (std::vector<ScaleKey*>::const_iterator it = scalekeys.begin(); it != scalekeys.end(); ++it)
		{
			if (clip_vec->time < (*it)->time)
				continue;
			else if (it == scalekeys.end() - 1)
			{
				return (*scalekeys.end() - 1)->scale;
			}
			else
			{
				actual_pos = (*it)->scale;
				actual_time = (*it)->time;
				next_pos = (*(it + 1))->scale;
				next_time = (*(it + 1))->time;
				break;
			}
		}
		//if no interpolation get clip 0 

		float weight = (clip_vec->time - actual_time) / (next_time - actual_time);

		return actual_pos.Lerp(next_pos, weight);

	}
	return scalekeys[0]->scale;
}
