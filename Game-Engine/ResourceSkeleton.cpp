#include "ResourceSkeleton.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
ResourceSkeleton::ResourceSkeleton(int UID) : Resource(UID, Resource_Texture)
{
}

ResourceSkeleton::~ResourceSkeleton()
{
}

void ResourceSkeleton::ReimportResource()
{
	//App->geometryloader->UnloadTexture(textureID);
}

bool ResourceSkeleton::LoadInMemory()
{
	/*textureID = App->geometryloader->LoadMaterial(exportedFile.c_str());
	if (textureID == 0)
	{
	return false;
	}*/
	return true;
}

bool ResourceSkeleton::UnloadFromMemory()
{
	//App->geometryloader->UnloadTexture(textureID);
	//textureID = 0;
	return true;
}

