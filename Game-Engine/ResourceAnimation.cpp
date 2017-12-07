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

