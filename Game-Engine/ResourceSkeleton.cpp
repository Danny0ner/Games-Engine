#include "ResourceSkeleton.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
ResourceSkeleton::ResourceSkeleton(int UID) : Resource(UID, Resource_Skeleton)
{

}

ResourceSkeleton::~ResourceSkeleton()
{
	for (std::vector<MeshBone*>::iterator temp = MeshBones.begin(); temp != MeshBones.end(); temp++)
	{
		RELEASE((*temp));
	}
}

void ResourceSkeleton::ReimportResource()
{
	//App->geometryloader->UnloadTexture(textureID);
}

bool ResourceSkeleton::LoadInMemory()
{
	App->geometryloader->LoadSkeleton(exportedFile.c_str(), this);
	return true;
}

bool ResourceSkeleton::UnloadFromMemory()
{
	//App->geometryloader->UnloadTexture(textureID);
	//textureID = 0;
	return true;
}

