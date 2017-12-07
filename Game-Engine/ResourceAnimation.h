#pragma once

#include "Resource.h"
#include "MathGeo\MathGeoLib.h"

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

struct Bone
{
	std::string name;

	std::vector<PositionKey*> positionkeys;
	std::vector<RotationKey*> rotationkeys;
	~Bone()
	{
		for (std::vector<PositionKey*>::iterator temp = positionkeys.begin(); temp != positionkeys.end(); temp++)
		{
			RELEASE((*temp));
		}
		for (std::vector<RotationKey*>::iterator temp = rotationkeys.begin(); temp != rotationkeys.end(); temp++)
		{
			RELEASE((*temp));
		}
	};
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