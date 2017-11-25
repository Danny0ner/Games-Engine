#pragma once

#include "Resource.h"

class ResourceAnimation : public Resource
{
public:
	ResourceAnimation(int UID);
	~ResourceAnimation();

	void ReimportResource();
	bool LoadInMemory()override;
	bool UnloadFromMemory()override;

public:
	uint textureID = 0;
};
