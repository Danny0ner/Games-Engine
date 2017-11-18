#include "ResourceTexture.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "mmgr\mmgr.h"
ResourceTexture::ResourceTexture(int UID): Resource(UID, Resource_Texture)
{
}

ResourceTexture::~ResourceTexture()
{
}

void ResourceTexture::ReimportResource()
{
	App->geometryloader->UnloadTexture(textureID);

}

bool ResourceTexture::LoadInMemory()
{
	textureID = App->geometryloader->LoadMaterial(exportedFile.c_str());
	if (textureID == 0)
	{
		return false;
	}
	return true;
}

bool ResourceTexture::UnloadFromMemory()
{
	App->geometryloader->UnloadTexture(textureID);
	textureID = 0;
	return true;
}

