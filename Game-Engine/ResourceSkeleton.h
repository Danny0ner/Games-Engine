#pragma once

#include "Resource.h"
#include <vector>


struct VertexWeight
{
	int VertexID = 0;

	float Weight = 0.0f;
};

struct MeshBone
{
	std::string name;
	std::vector<VertexWeight*> VertexWeights;

	~MeshBone()
	{
		for (std::vector<VertexWeight*>::iterator temp = VertexWeights.begin(); temp != VertexWeights.end(); temp++)
		{
			RELEASE((*temp));
		}
	}
};

class ResourceSkeleton : public Resource
{
public:
	ResourceSkeleton(int UID);
	~ResourceSkeleton();

	void ReimportResource();
	bool LoadInMemory()override;
	bool UnloadFromMemory()override;

public:
	std::vector<MeshBone*> MeshBones;
};

