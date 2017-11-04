#include "GeometryLoader.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "Glew\include\glew.h"
#include "MathGeoLib\Math\Quat.h"
#include "Assimp\include\cimport.h" 
#include "Assimp\include\scene.h" 
#include "Assimp\include\postprocess.h" 
#include "Assimp\include\cfileio.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib" ) /* Loading Devil lib */
#pragma comment (lib, "Devil/libx86/ILU.lib" ) /* Loading ILU lib */
#pragma comment (lib, "Devil/libx86/ILUT.lib" ) 
#pragma comment (lib, "Assimp/libx86/assimp.lib")

GeometryLoader::GeometryLoader(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Geometry Loader";
}

GeometryLoader::~GeometryLoader()
{
}

bool GeometryLoader::Init()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);

	ilutRenderer(ILUT_OPENGL);
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	return true;
}

GameObject* GeometryLoader::LoadGameObject(const char* fullPath)
{
	GameObject* newObject = new GameObject();

	uint length = strlen(fullPath);

	std::string namePath = fullPath;

	uint i = namePath.find_last_of("\\");
	char* testM = new char[length - i + 1];
	length = length - i;
	namePath.copy(testM, length, i);
	testM[length] = '\0';
	newObject->SetName(testM);

	delete[] testM;
	testM = nullptr;

	const aiScene* scene = aiImportFile(fullPath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		LOG("Scene %s loaded succesfully", fullPath);

		//Load transform
		aiNode* node = scene->mRootNode;
		newObject->AddComponent(LoadTransform(node));

		
		LOG("Loading meshes");

		newObject->AddChild(AddGameObjectChild(node, scene, newObject));

		aiReleaseImport(scene);

		return newObject;
	}
	else
	{
		LOG("Error loading scene %s", fullPath);
		return nullptr;
	}
}

GameObject * GeometryLoader::AddGameObjectChild(aiNode * node, const aiScene * scene, GameObject * parent)
{
	for (uint i = 0; i < node->mNumMeshes; i++)
	{
		GameObject* newObject = new GameObject(parent);
		aiMatrix4x4 matrix = node->mTransformation;
		newObject->AddComponent(LoadTransform(node));
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		newObject->SetName(node->mName.C_Str());
		LoadMesh(mesh ,node, scene, newObject);
		App->editor->GetQuadtree()->Insert(newObject);
		App->editor->Static_Vector.push_back(newObject);
		return newObject;
	}
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		parent->AddChild(AddGameObjectChild(node->mChildren[i], scene, parent));
	}
	return nullptr;
}

/*
void GeometryLoader::LoadNewTexture(const char* fullPath)
{
	int count = 0;
	for (int i = 0; i < App->editor->GetRoot()->childs.size(); i++)
	{
		count = 0;
		for (int j = 0; j < App->editor->GetRoot()->childs[i]->components.size(); j++)
		{
			if (App->editor->GetRoot()->childs[i]->components[j]->GetType() == Component_Material)
			{
				dynamic_cast<ComponentMaterial*>(App->editor->GetRoot()->childs[i]->components[j])->OverrideTexture(fullPath);
				count++;
			}
		}

		//In case it didn't have any previous material
		if (count == 0)
		{
			ComponentMaterial* newMat = new ComponentMaterial();
			newMat->idTexture = App->textures->ImportImage(fullPath);
			newMat->SetName(fullPath);
			App->sceneEditor->GetRoot()->childs[i]->AddComponent(newMat);
		}
	}
	LOG("Set %s as new texture for current meshes.");
}*/

CompMesh* GeometryLoader::LoadMesh(aiMesh* mesh ,aiNode* node, const aiScene* scene, GameObject* addTo)
{
	CompMesh* m = new CompMesh;

		if (mesh != nullptr)
		{
			//VERTICES
			m->numVertices = mesh->mNumVertices;

			m->vertices = new float[m->numVertices * 3];

			memcpy(m->vertices, mesh->mVertices, sizeof(float)* m->numVertices * 3);

			LOG("New mesh with %d vertices", m->numVertices);

			glGenBuffers(1, (GLuint*)&m->idVertices);
			glBindBuffer(GL_ARRAY_BUFFER, m->idVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->numVertices * 3, m->vertices, GL_STATIC_DRAW);

			//INDICES
			if (mesh->HasFaces())
			{
				m->numIndices = mesh->mNumFaces * 3;
				m->indices = new uint[m->numIndices];

				for (uint i = 0; i < mesh->mNumFaces; ++i)
				{
					if (mesh->mFaces[i].mNumIndices != 3)
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else
					{
						memcpy(&m->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}

				glGenBuffers(1, (GLuint*)&m->idIndices);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->idIndices);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * m->numIndices, m->indices, GL_STATIC_DRAW);
			}

			//NORMALS
			if (mesh->HasNormals())
			{
				m->normals = new float[m->numVertices * 3];
				memcpy(m->normals, mesh->mNormals, sizeof(float) * m->numVertices * 3);

				glGenBuffers(1, (GLuint*) &(m->idNormals));
				glBindBuffer(GL_ARRAY_BUFFER, m->idNormals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->numVertices * 3, m->normals, GL_STATIC_DRAW);
			}

			//COLORS
			if (mesh->HasVertexColors(0))
			{
				m->colors = new float[m->numVertices * 3];
				memcpy(m->colors, mesh->mColors, sizeof(float) * m->numVertices * 3);

				glGenBuffers(1, (GLuint*) &(m->idColors));
				glBindBuffer(GL_ARRAY_BUFFER, m->idColors);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->numVertices * 3, m->colors, GL_STATIC_DRAW);
			}

			//TEXTURE COORDS
			if (mesh->HasTextureCoords(0))
			{
				m->texCoords = new float[m->numVertices * 3];
				memcpy(m->texCoords, mesh->mTextureCoords[0], sizeof(float) * m->numVertices * 3);

				glGenBuffers(1, (GLuint*) &(m->idTexCoords));
				glBindBuffer(GL_ARRAY_BUFFER, m->idTexCoords);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m->numVertices * 3, m->texCoords, GL_STATIC_DRAW);

				aiMaterial* material = nullptr;
				material = scene->mMaterials[mesh->mMaterialIndex];
				addTo->AddComponent(LoadMaterial(material));

			}

			m->SetName(node->mName.C_Str());

			m->enclosingBox.SetNegativeInfinity();

			m->enclosingBox.Enclose((float3*)m->vertices, m->numVertices);

			addTo->AddComponent(m);
		}
	return nullptr;
}

uint GeometryLoader::ImportImage(const char * image)
{
	ILuint imageID;				// Create an image ID as a ULuint

	GLuint textureID;			// Create a texture ID as a GLuint

	ILboolean success;			// Create a flag to keep track of success/failure

	ILenum error;				// Create a flag to keep track of the IL error state

	ilGenImages(1, &imageID); 		// Generate the image ID

	ilBindImage(imageID); 			// Bind the image

	success = ilLoadImage(image); 	// Load the image file

									// If we managed to load the image, then we can start to do things with it...
	if (success)
	{
		// If the image is flipped (i.e. upside-down and mirrored, flip it the right way up!)
		ILinfo ImageInfo;
		iluGetImageInfo(&ImageInfo);
		if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
		{
			iluFlipImage();
		}

		// Convert the image into a suitable format to work with
		// NOTE: If your image contains alpha channel you can replace IL_RGB with IL_RGBA
		success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE);

		// Quit out if we failed the conversion
		if (!success)
		{
			error = ilGetError();
			LOG("Image conversion failed - IL reports error: %s ", iluErrorString(error));
			exit(-1);
		}

		// Generate a new texture
		glGenTextures(1, &textureID);

		// Bind the texture to a name
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Set texture clamping method
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

		// Set texture interpolation method to use linear interpolation (no MIPMAPS)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		// Specify the texture specification
		glTexImage2D(GL_TEXTURE_2D, 				// Type of texture
			0,				// Pyramid level (for mip-mapping) - 0 is the top level
			ilGetInteger(IL_IMAGE_FORMAT),	// Internal pixel format to use. Can be a generic type like GL_RGB or GL_RGBA, or a sized type
			ilGetInteger(IL_IMAGE_WIDTH),	// Image width
			ilGetInteger(IL_IMAGE_HEIGHT),	// Image height
			0,				// Border width in pixels (can either be 1 or 0)
			ilGetInteger(IL_IMAGE_FORMAT),	// Format of image pixel data
			GL_UNSIGNED_BYTE,		// Image data type
			ilGetData());			// The actual image data itself

	}
	else // If we failed to open the image file in the first place...
	{
		error = ilGetError();
		LOG("Image load failed - IL reports error: %s", iluErrorString(error));
		return 0;
	}

	ilDeleteImages(1, &imageID); // Because we have already copied image data into texture data we can release memory used by image.

	LOG("Texture creation successful.");

	return textureID; // Return the GLuint to the texture so you can use it!
}

CompMaterial* GeometryLoader::LoadMaterial(aiMaterial* newMaterial)

{
	CompMaterial* m = new CompMaterial;
	//MATERIAL

	if (newMaterial != nullptr)
	{
		uint numTextures = newMaterial->GetTextureCount(aiTextureType_DIFFUSE);
		aiString path;

		newMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &path);

		uint length = path.length;

		std::string namePath = path.C_Str();

		uint i = namePath.find_last_of("\\");
		length = length - i - 1;
		char* lastpath = new char[length + 1];
	
		namePath.copy(lastpath, length, i + 1);
		lastpath[length] = '\0';
		std::string fullPath = "Models&Textures/";
		uint l = fullPath.size();
		fullPath.append(lastpath);
		m->idTexture = ImportImage(fullPath.c_str());
		m->SetName(lastpath);

		delete[] lastpath;
		lastpath = nullptr;
		return m;
	}
	return nullptr;
}

CompTransform* GeometryLoader::LoadTransform(aiNode* node)
{
	aiVector3D translation;
	aiVector3D scale;
	aiQuaternion q1;
	node->mTransformation.Decompose(scale, q1, translation);

	float3 pos(translation.x, translation.y, translation.z);
	float3 sca(scale.x, scale.y, scale.z);

	Quat rot(q1.x, q1.y, q1.z, q1.w);

	return new CompTransform(pos, sca, rot);
}


bool GeometryLoader::CleanUp()
{
	return true;
}