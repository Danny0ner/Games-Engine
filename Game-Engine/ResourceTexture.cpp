#include "ResourceTexture.h"
#include "Application.h"
#include "GeometryLoader.h"

ResourceTexture::ResourceTexture(int UID): Resource(UID, Resource_Texture)
{
}

ResourceTexture::~ResourceTexture()
{
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

void ResourceTexture::Save(Configuration & config) const
{
}

void ResourceTexture::Load(Configuration & config)
{
}
