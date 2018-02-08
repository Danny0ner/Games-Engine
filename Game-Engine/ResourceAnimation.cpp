#include "ResourceAnimation.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
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
}

void Bone::UpdateBone(float time, const std::vector<AnimationClip*>& clip_vec)
{
	float3 pos = GetPosition(time, clip_vec);
	Quat rot = GetRotation(time, clip_vec);
	float3 scale = GetScale(time, clip_vec);
	
	//float4x4::FromTRS()
}

float3 Bone::GetPosition(float time, const std::vector<AnimationClip*>& clip_vec)
{
	//for(std::vector<PositionKey*>::iterator )
	if (positionkeys[clip_vec[0]->actualpos[1]]->time <= time)
	{
		clip_vec[0]->actualpos[0]++;
		clip_vec[0]->actualpos[1]++;
	}
	
	//if no interpolation get clip 0 

	float weight = (time- positionkeys[clip_vec[0]->actualpos[0]]->time)/ (positionkeys[clip_vec[0]->actualpos[1]]->time - positionkeys[clip_vec[0]->actualpos[0]]->time);



	return positionkeys[clip_vec[0]->actualpos[0]]->position.Lerp(positionkeys[clip_vec[0]->actualpos[1]]->position, weight);
}

Quat Bone::GetRotation(float time, const std::vector<AnimationClip*>& clip_vec)
{
}

float3 Bone::GetScale(float time, const std::vector<AnimationClip*>& clip_vec)
{
}
