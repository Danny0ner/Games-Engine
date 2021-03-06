#pragma once

#include "Resource.h"

class ResourceTexture : public Resource
{
public:
	ResourceTexture(int UID);
	~ResourceTexture();

	void ReimportResource();
	bool LoadInMemory()override;
	bool UnloadFromMemory()override;

public:
	uint textureID = 0;
};