#include <experimental\filesystem>
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
	for (std::map<int, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
			delete it->second;
	}
}

bool ResourcesManager::Start()
{
	checkingTimer.Start();
	return true;
}

update_status ResourcesManager::Update(float dt)
{
	std::string yeahyo;
	if (checkingTimer.Read() > 3000)
	{
		for (std::experimental::filesystem::recursive_directory_iterator::value_type p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
		{
			App->Console.AddLog(p.path().string().c_str());
			if (std::experimental::filesystem::is_directory(p))
			{
				std::string ye = "Library/";
				ye += p.path().filename().string();
				std::experimental::filesystem::create_directory(ye.c_str());
			}
		}
		checkingTimer.Start();
	}


	return UPDATE_CONTINUE;
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

		std::string tmpPath = fileName;
		int length = tmpPath.length();
		uint i = tmpPath.find_last_of("/");
		length = length - i - 1;
		char* tmp = new char[length + 1];
		tmpPath.copy(tmp, length, i + 1);
		tmp[length] = '\0';
		std::string exFile = tmp;
		delete[] tmp;

		switch (type)
		{
			case Resource_Texture:
			{
				imported = App->geometryloader->ImportImage(fileName, exFile);
				break;
			}
		}

		if (imported == true)
		{
			Resource* newResource = CreateNewResource(type, UID);
			newResource->file = fileName;
			newResource->exportedFile = "Library/Material/";
			newResource->exportedFile += exFile;
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

		imported = App->geometryloader->SaveMeshToOwnFormat(mesh, meshName);

		if (imported == true)
		{
			Resource* newResource = CreateNewResource(Resource_Mesh, UID);
			newResource->file = meshName;
			newResource->exportedFile = "Library/Mesh/";
			newResource->exportedFile += meshName;
			newResource->exportedFile += ".DarkyHijo";
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
			ret = (Resource*) new ResourceMesh(UID);
			break;
		}
	}

	if (ret != nullptr)
	{
		resources[UID] = ret; //Put it on the map
	}

	return ret;
}

void ResourcesManager::SaveResources(Configuration& save) const
{
	for (std::map<int, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		(it)->second->Save(save);
	}
}

void ResourcesManager::LoadResources(Configuration& resources)
{
	uint re = resources.GetArraySize("Scene Resources");
	for (int i = 0; i < re; i++)
	{
		Configuration tmpConfig = resources.GetArray("Scene Resources", i);
		int tmpUID = tmpConfig.GetInt("UID");
		if (Get(tmpUID) == nullptr)
		{
			switch (tmpConfig.GetInt("Type"))
			{
			case Resource_Mesh:
			{
				ResourceMesh* resourceMesh = (ResourceMesh*)CreateNewResource(Resource_Mesh, tmpUID);
				resourceMesh->Load(tmpConfig);
				break;
			}
			case Resource_Texture:
			{
				ResourceTexture* resourceTexture = (ResourceTexture*)CreateNewResource(Resource_Texture, tmpUID);
				resourceTexture->Load(tmpConfig);
				break;
			}
			}
		}
	}
}


