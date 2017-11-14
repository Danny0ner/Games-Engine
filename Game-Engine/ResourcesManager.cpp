#include "ResourcesManager.h"
#include "Application.h"

#include "ResourceTexture.h"

#include "Assimp\include\cimport.h" 
#include "Assimp\include\scene.h" 
#include "Assimp\include\postprocess.h" 
#include "Assimp\include\cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

ResourcesManager::ResourcesManager(Application* app, bool startEnabled) : Module(app, startEnabled)
{
	name = "resources";
}

ResourcesManager::~ResourcesManager()
{
}

int ResourcesManager::Find(const char * fileName)
{
	for (std::map<int, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetFile(), fileName) == 0)
		{
			return it->first;
		}
	}
	return 0;
}

int ResourcesManager::ImportFile(const char * fileName, ResourceType type)
{
	//Check that the file isn't already loaded
	int UID = Find(fileName);

	if (UID == 0)
	{
		bool imported;
		UID = App->RandomUIDGen->Int();
		std::string outfile = std::to_string(UID);

		switch (type)
		{
			case Resource_Texture:
			{
				imported = App->geometryloader->ImportImage(fileName, outfile);
				break;
			}
		}

		if (imported == true)
		{
			Resource* newResource = CreateNewResource(type, UID);
			newResource->file = fileName;
			newResource->exportedFile = "Library/Material/";
			newResource->exportedFile += outfile;
			newResource->exportedFile += ".dds";
			return UID;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return UID;
	}
}

int ResourcesManager::ImportFile(const char* meshName, aiMesh * mesh)
{
	int UID = Find(meshName);

	if (UID == 0)
	{
		bool imported;
		int UID = App->RandomUIDGen->Int();
		std::string exFile = std::to_string(UID);

		//save mesh

		if (imported == true)
		{
			Resource* newResource = CreateNewResource(Resource_Mesh, UID);
			newResource->file = meshName;
			newResource->exportedFile = "Library/Mesh/";
			newResource->exportedFile += exFile;
			newResource->exportedFile += ".don";
			return UID;
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return UID;
	}
}

Resource * ResourcesManager::Get(int UID)
{
	std::map<int, Resource*>::iterator it = resources.find(UID);
	if (it != resources.end())
		return it->second;
	return nullptr;
}

Resource * ResourcesManager::CreateNewResource(ResourceType type, int UID)
{
	Resource* ret = nullptr;

	switch (type)
	{
	case Resource_Texture:
	{
		ret = (Resource*) new ResourceTexture(UID);
		break;
	}
	case Resource_Mesh:
	{
		break;
	}
	}

	if (ret != nullptr)
	{
		resources[UID] = ret; //Put it on the map
	}

	return ret;
}