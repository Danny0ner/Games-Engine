#pragma once

#include "Resource.h"
#include "MathGeo\MathGeoLib.h"

class GameObject;
struct AnimationClip;

struct PositionKey 
{
	float time;
	float3 position;
};

struct RotationKey
{
	float time;
	Quat rotation;
};

struct ScaleKey
{
	float time;
	float3 scale;
};

class Bone
{
public:
	void UpdateBone(GameObject* gameobject, std::vector<AnimationClip*>& clip_vec) const;
	~Bone();

	float3 GetPosition(AnimationClip* clip_vec) const;
	Quat GetRotation(AnimationClip* clip_vec) const;
	float3 GetScale(AnimationClip* clip_vec) const;

	std::string name;

	std::vector<PositionKey*> positionkeys;
	std::vector<RotationKey*> rotationkeys;
	std::vector<ScaleKey*> scalekeys;
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