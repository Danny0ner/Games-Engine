#pragma once

#include "Application.h"
#include <map>
#include "Globals.h"
#include "Resource.h"
#include "CompMaterial.h"
#include "CompMesh.h"
#include "CompAnimation.h"

class aiMesh;
class aiAnimation;
class ResourcesManager : public Module
{
public:
	ResourcesManager(Application* app, bool startEnabled = true);
	~ResourcesManager();
	
	bool Start();
	update_status Update(float dt);
	
	
	int Find(const char* fileName);
	
	int ImportFile(const char* meshName, aiMesh* mesh);
	int SaveAnimationResource(const char * animationname);
	Resource* Get(int UID);
	Resource* GetResourceByName(const char * fileName);
	int ImportFileImage(const char * fileName);
	int ImportAnimation(aiAnimation* anim);
	int ImportSkeleton(const char* meshName, aiMesh* mesh);


	Resource* CreateNewResource(ResourceType type, int UID);
	void ShowTextureResources(CompMaterial* material);
	void ShowAnimationResources(CompAnimation * material);
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