#pragma once

#include "Resource.h"

class ResourceSkeleton : public Resource
{
public:
	ResourceSkeleton(int UID);
	~ResourceSkeleton();

	void ReimportResource();
	bool LoadInMemory()override;
	bool UnloadFromMemory()override;

public:
	uint textureID = 0;
};

