#pragma once

#include "Resource.h"
#include "MathGeo\MathGeoLib.h"

struct AnimationClip;

struct PositionKey 
{
	float time;
	float3 position;
};

struct RotationKey
{
	float time;
	float4 rotation;
};

class Bone
{
public:
	void UpdateBone(float time, const std::vector<AnimationClip*>& clip_vec);
	~Bone();

private:

	float3 GetPosition(float time, const std::vector<AnimationClip*>& clip_vec);
	Quat GetRotation(float time, const std::vector<AnimationClip*>& clip_vec);
	float3 GetScale(float time, const std::vector<AnimationClip*>& clip_vec);

	GameObject* linked_go = nullptr;

	std::string name;

	std::vector<PositionKey*> positionkeys;
	std::vector<RotationKey*> rotationkeys;
};

class ResourceAnimation : public Resource
{
public:
	ResourceAnimation(int UID);
	~ResourceAnimation();

	void ReimportResource();
	bool LoadInMemory()override;
	bool UnloadFromMemory()override;

public:
	std::string name;
	float duration;
	float ticksPerSec;

	std::vector<Bone*> bones;
};