#pragma once

#include "Application.h"
#include <map>
#include "Globals.h"
#include "Resource.h"

class aiMesh;

class ResourcesManager : public Module
{
public:
	ResourcesManager(Application* app, bool startEnabled = true);
	~ResourcesManager();
	int Find(const char* fileName);
	int ImportFile(const char* fileName, ResourceType type = Resource_Unknown);
	int ImportFile(const char* meshName, aiMesh* mesh);
	Resource* Get(int UID);
	Resource* CreateNewResource(ResourceType type, int UID);

private:
	std::map<int, Resource*> resources;
};