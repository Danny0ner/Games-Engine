#pragma once

#include "Globals.h"
#include "Application.h"
#include "Module.h"

enum FileType
{
	fileMesh,
	fileMaterial,
	fileScene,
	fileAnimation,
	fileSkeleton
};

class FileSystem : public Module
{
public:
	FileSystem(Application* app, bool startEnabled = true);
	~FileSystem();

	bool Init();

	bool CreateNewDirectory(const char* name);
	void SaveFile(const char* name, char* buffer, int bufferSize, FileType type) const;
	bool LoadFile(const char* name, char** buffer, int& size, FileType type) const;
};