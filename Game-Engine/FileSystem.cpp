#include "FileSystem.h"
#include "Application.h"
#include <fstream>
#include "mmgr\mmgr.h"


FileSystem::FileSystem(Application * app, bool startEnabled) : Module(app, startEnabled)
{
	name = "file system";
}

FileSystem::~FileSystem()
{
}

bool FileSystem::Init()
{
	CreateNewDirectory("Assets");
	CreateNewDirectory("Library");
	CreateNewDirectory(MESH_DIRECTORY);
	CreateNewDirectory(MATERIAL_DIRECTORY);
	CreateNewDirectory(SCENE_DIRECTORY);
	CreateNewDirectory(ANIM_DIRECTORY);

	return true;
}

bool FileSystem::CreateNewDirectory(const char * name)
{
	if (CreateDirectory(name, NULL))
	{
		LOG("Created %s folder.", name);
		return true;
	}
	else
	{
		LOG("Couldn't create %s folder.", name);
		return false;
	}
}

void FileSystem::SaveFile(const char * name, char * buffer, int bufferSize, FileType type) const
{
	std::string path;
	if (type == fileMesh)
	{
		path = MESH_DIRECTORY;
		path += "/";
		path += name;
		path += MESH_EXTENSION;
	}
	else if (type == fileMaterial)
	{
		path = MATERIAL_DIRECTORY;
		path += "/";
		path += name;
		path += MATERIAL_EXTENSION;
	}
	else if (type == fileScene)
	{
		path += SCENE_DIRECTORY;
		path += "/";
		path += name;
		path += SCENE_EXTENSION;
	}
	else if (type == fileAnimation)
	{
		path += ANIM_DIRECTORY;
		path += "/";
		path += name;
		path += ANIM_EXTENSION;
	}
	std::ofstream file(path.c_str(), std::ofstream::out | std::ofstream::binary);
	file.write(buffer, bufferSize);
	file.close();
}

bool FileSystem::LoadFile(const char * name, char ** buffer, int& size, FileType type) const
{
	bool ret = false;

	std::string path;
	if (type == fileMesh)
	{
		path = MESH_DIRECTORY;
		path += "/";
		path += name;
		path += MESH_EXTENSION;
	}
	else if (type == fileMaterial)
	{
		path = MATERIAL_DIRECTORY;
		path += "/";
		path += name;
		path += MATERIAL_EXTENSION;
	}
	else if (type == fileScene)
	{
		path += SCENE_DIRECTORY;
		path += "/";
		path += name;
		path += SCENE_EXTENSION;
	}

	std::ifstream file(path, std::ifstream::binary);

	if (file.good())
	{
		if (file.is_open())
		{
			file.seekg(0, file.end);
			std::streamsize length = file.tellg();
			file.seekg(0, file.beg);

			*buffer = new char[length];

			file.read(*buffer, length);

			if (file)
			{
				LOG("File %s loaded succesfully", name);
				ret = true;
			}
			else
			{
				LOG("Couldn't load %s", name);
			}
			file.close();
		}
	}
	return ret;
}