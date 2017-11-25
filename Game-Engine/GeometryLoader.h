#pragma once
#include "Application.h"
#include <string>
#include <vector>
#include "Globals.h"
#include "ResourceMesh.h"
#include "Assimp\include\cimport.h"
#include "Assimp\include\scene.h"
#include "Assimp\include\postprocess.h"
#include "Assimp\include\cfileio.h"
class GameObject;

class CompMesh;
class CompMaterial;
class CompTransform;

class GeometryLoader : public Module
{
public:
	GeometryLoader(Application* app, bool start_enabled = true);
	~GeometryLoader();

	bool Init();
	bool CleanUp();
	void ImportFBX(const char* fbxName);
	void ImportFBXNode(aiNode* node, const aiScene* scene);
	void ImportImageResource(const char * image, std::string & output_file);
	void ImportImageResourceFromDragAndDrop(const char * image, std::string & output_file);
	GameObject* LoadGameObject(const char* fullPath);
	GameObject* AddGameObjectChild(aiNode* node, const aiScene* scene, GameObject* addTo);
	CompMaterial* LoadMaterial(aiMaterial* newMaterial);
	CompTransform* LoadTransform(aiNode* node);
	void ImportAnimation(const aiAnimation* scene);
	uint ImportImage(const char* path, std::string& output_file);
	void UnloadTexture(uint id);
	bool SaveMeshToOwnFormat(const aiMesh* mesh, const char * outputFile);
	void LoadMeshOwnFormat(const char * inputFile, ResourceMesh* mesh);
	bool SaveTextureToOwnFormat(const char * path, char * buffer, int buffer_size);
	uint LoadMaterial(const char * image);
};
