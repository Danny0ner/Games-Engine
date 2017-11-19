#pragma once

#include "Application.h"
#include <map>
#include "Globals.h"
#include "Resource.h"
#include "CompMaterial.h"
#include "CompMesh.h"
class aiMesh;

class ResourcesManager : public Module
{
public:
	ResourcesManager(Application* app, bool startEnabled = true);
	~ResourcesManager();
	
	bool Start();
	update_status Update(float dt);
	
	
	int Find(const char* fileName);
	
	int ImportFile(const char* fileName, ResourceType type = Resource_Unknown);
	int ImportFileDropped(const char * fileName, ResourceType type);
	int ImportFile(const char* meshName, aiMesh* mesh);
	Resource* Get(int UID);
	Resource* GetResourceByName(const char * fileName);
	Resource* CreateNewResource(ResourceType type, int UID);
	void ShowTextureResources(CompMaterial* material);
	void ShowMeshResources(CompMesh * material);
	//void ShowMeshResources(CompMaterial* material);
	void ReimportFile(const char* filename);
	void CheckResources();
	void SaveResources(Configuration & save) const;
	void ShowAssetsFolder();
	void LoadResources(Configuration & resources);

private:
	std::map<int, Resource*> resources;

	Timer checkingTimer;
};