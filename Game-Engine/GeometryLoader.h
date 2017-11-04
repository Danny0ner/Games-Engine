#pragma once
#include "Application.h"
#include <string>
#include <vector>
#include "Globals.h"

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

	GameObject* LoadGameObject(const char* fullPath);
	GameObject* AddGameObjectChild(aiNode* node, const aiScene* scene, GameObject* addTo);
	CompMesh* LoadMesh(aiMesh* mesh,aiNode* node, const aiScene* scene, GameObject* addTo);
	CompMaterial* LoadMaterial(aiMaterial* newMaterial);
	CompTransform* LoadTransform(aiNode* node);

	uint ImportImage(const char* path);
	void LoadNewTexture(const char* fullPath);

	void Load(CompMesh* mesh, char* buffer);
	void Save(CompMesh* mesh);
};
