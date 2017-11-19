#include <experimental\filesystem>
#include "ResourcesManager.h"
#include "GeometryLoader.h"
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
	for (std::experimental::filesystem::recursive_directory_iterator::value_type p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
	{
		if (strcmp(p.path().filename().extension().string().c_str(),".FBX") == 0 || strcmp(p.path().filename().extension().string().c_str(),".fbx") == 0)
		{
			App->geometryloader->ImportFBX(p.path().string().c_str());
		}
		else if (strcmp(p.path().filename().extension().string().c_str(), ".PNG") == 0 || strcmp(p.path().filename().extension().string().c_str(), ".png") == 0 || strcmp(p.path().filename().extension().string().c_str(), ".tga") == 0 || strcmp(p.path().filename().extension().string().c_str(), ".TGA") == 0)
		{
			std::string AssetsPath = "Assets/";
			AssetsPath.append(p.path().filename().string().c_str());
			App->geometryloader->ImportImageResource(AssetsPath.c_str(), p.path().filename().string());
		}
	}

	return true;
}

update_status ResourcesManager::Update(float dt)
{
	std::string yeahyo;
	if (checkingTimer.Read() > 3000)
	{
		CheckResources();
		checkingTimer.Start();
	}


	return UPDATE_CONTINUE;
}

void ResourcesManager::CheckResources()
{
	for (std::map<int, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		for (std::experimental::filesystem::recursive_directory_iterator::value_type p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
		{
			ResourceTexture* tmpres = nullptr;

			std::experimental::filesystem::file_time_type ftime;
			std::time_t cftime;
			std::string fileWriteTime;

			std::string ye = p.path().filename().string();
			if (strcmp(it->second->file.c_str(), ye.c_str()) == 0)
			{
				switch (it->second->GetType())
				{
				case Resource_Texture:
					ftime = std::experimental::filesystem::last_write_time(p.path());
					cftime = decltype(ftime)::clock::to_time_t(ftime);
					fileWriteTime = std::asctime(std::localtime(&cftime));

					if (strcmp(it->second->LastWriteTime.c_str(), fileWriteTime.c_str()) != 0)
					{
						tmpres = (ResourceTexture*)it->second;
						tmpres->UnloadFromMemory();
						ReimportFile(tmpres->file.c_str());
						if (tmpres->GetReferenceCount() > 0)
						{
							tmpres->LoadInMemory();
						}
					}
					break;
				case Resource_Mesh:
					break;
				}
			}
		}
	}
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

Resource* ResourcesManager::GetResourceByName(const char * fileName)
{
	for (std::map<int, Resource*>::iterator it = resources.begin(); it != resources.end(); ++it)
	{
		if (strcmp(it->second->GetFile(), fileName) == 0)
		{
			return it->second;
		}
	}
	return nullptr;
}

int ResourcesManager::ImportFile(const char * fileName, ResourceType type)
{
	//Check that the file isn't already loaded

	std::string assetsfile = fileName;
	std::string tmpPath = fileName;
	int length = tmpPath.length();
	uint i = tmpPath.find_last_of("/");
	uint y = tmpPath.find_last_of("\\");
	if (i > 1000) i = y; //trying to discard undefined number
	length = length - i - 1;
	char* tmp = new char[length + 1];
	tmpPath.copy(tmp, length, i + 1);
	tmp[length] = '\0';
	std::string exFile = tmp;
	delete[] tmp;

	int UID = Find(exFile.c_str());

	if (UID == 0)
	{
		bool imported;
		UID = App->RandomUIDGen->Int();
	

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
			newResource->file = exFile;
			newResource->exportedFile = "Library/Material/";
			newResource->exportedFile += exFile;
			newResource->exportedFile += ".dds";

			for (std::experimental::filesystem::recursive_directory_iterator::value_type file_in_path : std::experimental::filesystem::recursive_directory_iterator("Assets"))
			{
				if (std::experimental::filesystem::is_regular_file(file_in_path.path()))
				{
					if (strcmp(newResource->file.c_str(), file_in_path.path().filename().string().c_str()) == 0)
					{
						std::experimental::filesystem::file_time_type ftime = std::experimental::filesystem::last_write_time(file_in_path.path());
						std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
						newResource->LastWriteTime = std::asctime(std::localtime(&cftime));
					}
				}
			}

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

int ResourcesManager::ImportFileDropped(const char * fileName, ResourceType type)
{
	//Check that the file isn't already loaded

	std::string assetsfile = fileName;
	std::string tmpPath = fileName;
	int length = tmpPath.length();
	uint i = tmpPath.find_last_of("\\");
	length = length - i - 1;
	char* tmp = new char[length + 1];
	tmpPath.copy(tmp, length, i + 1);
	tmp[length] = '\0';
	std::string exFile = tmp;
	delete[] tmp;

	int UID = Find(exFile.c_str());

	if (UID == 0)
	{
		bool imported;
		UID = App->RandomUIDGen->Int();


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
			newResource->file = exFile;
			newResource->exportedFile = "Library/Material/";
			newResource->exportedFile += exFile;
			newResource->exportedFile += ".dds";

			for (std::experimental::filesystem::recursive_directory_iterator::value_type file_in_path : std::experimental::filesystem::recursive_directory_iterator("Assets"))
			{
				if (std::experimental::filesystem::is_regular_file(file_in_path.path()))
				{
					if (strcmp(newResource->file.c_str(), file_in_path.path().filename().string().c_str()) == 0)
					{
						std::experimental::filesystem::file_time_type ftime = std::experimental::filesystem::last_write_time(file_in_path.path());
						std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
						newResource->LastWriteTime = std::asctime(std::localtime(&cftime));
					}
				}
			}

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
			newResource->LastWriteTime = "nowritetime";

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

void ResourcesManager::ShowTextureResources(CompMaterial* material)
{
	for (std::experimental::filesystem::recursive_directory_iterator::value_type p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
	{
		if (strcmp(p.path().filename().extension().string().c_str(), ".PNG") == 0 || strcmp(p.path().filename().extension().string().c_str(), ".png") == 0)
		{
			ImGui::Text(p.path().filename().string().c_str());
			if (ImGui::IsItemClicked())
			{
				{
					if (material->GetResourceTex() != nullptr)
					{
						material->GetResourceTex()->UnloadFromComponent();
					}
					material->AddResourceByName(p.path().filename().string().c_str());
				}
			}
		}
	}
}


void ResourcesManager::ReimportFile(const char* filename)
{
	std::string tmpPath = filename;
	std::string fullPath = "Assets/";
	fullPath.append(filename);
	App->geometryloader->ImportImage(fullPath.c_str(), tmpPath);
}



void ResourcesManager::SaveResources(Configuration& save) const
{
	for (std::map<int, Resource*>::const_iterator it = resources.begin(); it != resources.end(); ++it)
	{
		(it)->second->Save(save);
	}
}

void ResourcesManager::ShowAssetsFolder()
{
	std::string temp = "Assets Folder";

	ImGui::Begin(temp.c_str(), 0, ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::BeginChild("Assets Files", ImVec2(0, 200), true, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		for (std::experimental::filesystem::recursive_directory_iterator::value_type p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
		{
			ImGui::Text(p.path().filename().string().c_str());
			if (ImGui::IsItemClicked())
			{
				if (strcmp(p.path().filename().extension().string().c_str(), ".FBX") == 0 || strcmp(p.path().filename().extension().string().c_str(), ".fbx") == 0)
				{
					App->editor->CreateNewGameObject(p.path().string().c_str());
				}
			}
		}
		ImGui::EndChild();

		ImGui::End();
}

void ResourcesManager::LoadResources(Configuration& resources)
{
	uint re = resources.GetArraySize("Scene Resources");
	for (int i = 0; i < re; i++)
	{
		Configuration tmpConfig = resources.GetArray("Scene Resources", i);
		int tmpUID = tmpConfig.GetInt("UID");
		std::string str = tmpConfig.GetString("File Name");
		if (Find(str.c_str()) == 0)
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


