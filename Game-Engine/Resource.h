#pragma once

#include "Globals.h"
#include <string>

class Configuration;

enum ResourceType
{
	Resource_Texture,
	Resource_Mesh,
	Resource_Animation,
	Resource_Skeleton,
	Resource_Unknown
};

class Resource
{
	friend class ResourcesManager;
public:
	Resource(int UID, ResourceType type);
	virtual ~Resource();

	ResourceType GetType()const;
	int GetUID()const;
	const char* GetFile()const;
	const char* GetExportedFile()const;
	bool IsLoadedToMemory()const;
	void LoadToComponent();
	void UnloadFromComponent();
	uint GetReferenceCount()const;

	virtual void ReimportResource(const char* filename);
	virtual void Save(Configuration& config) const;
	virtual void Load(Configuration& config);
	virtual bool LoadInMemory() = 0;
	virtual bool UnloadFromMemory() = 0;

protected:
	int UID = 0;
	std::string file;
	std::string exportedFile;
	std::string LastWriteTime;
	ResourceType type = Resource_Unknown;
	uint references = 0;
};