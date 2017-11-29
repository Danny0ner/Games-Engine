#include "ResourceAnimation.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
ResourceAnimation::ResourceAnimation(int UID) : Resource(UID, Resource_Texture)
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
	/*textureID = App->geometryloader->LoadMaterial(exportedFile.c_str());
	if (textureID == 0)
	{
		return false;
	}*/
	return true;
}

bool ResourceAnimation::UnloadFromMemory()
{
	//App->geometryloader->UnloadTexture(textureID);
	//textureID = 0;
	return true;
}

