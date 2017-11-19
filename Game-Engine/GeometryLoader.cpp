#include "GeometryLoader.h"
#include "ModuleEditor.h"
#include "GameObject.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "Glew\include\glew.h"
#include "MathGeo\Math\Quat.h"
#include "Assimp\include\cimport.h" 
#include "Assimp\include\scene.h" 
#include "Assimp\include\postprocess.h" 
#include "Assimp\include\cfileio.h"
#include "Devil\include\il.h"
#include "Devil\include\ilu.h"
#include "Devil\include\ilut.h"
#include "mmgr\mmgr.h"
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

void GeometryLoader::ImportFBX(const char* fbxName)
{
	uint length = strlen(fbxName);

	std::string namePath = fbxName;

	uint i = namePath.find_last_of("\\");
	char* fbxname = new char[length - i + 1];
	length = length - i;
	namePath.copy(fbxname, length, i);
	fbxname[length] = '\0';

	const aiScene* scene = aiImportFile(fbxName, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		LOG("Scene %s loaded succesfully", fbxName);

		//Load transform
		aiNode* node = scene->mRootNode;

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			scene->mMeshes[i]->mName = fbxname;
			scene->mMeshes[i]->mName.Append(std::to_string(i).c_str());
		}

		LOG("Loading meshes");

		ImportFBXNode(node, scene);

		aiReleaseImport(scene);

		delete[] fbxname;
		fbxname = nullptr;

	}
	else
	{
		LOG("Error loading scene %s", fbxName);

		delete[] fbxname;
		fbxname = nullptr;
	}
}

void GeometryLoader::ImportFBXNode(aiNode * node, const aiScene * scene)
{
	if (node->mNumMeshes != 0)
	{
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

			App->resources->ImportFile(mesh->mName.C_Str(), mesh);

		}
	}
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		ImportFBXNode(node->mChildren[i], scene);
	}
}

void GeometryLoader::ImportImageResource(const char * image, std::string& output_file)
{

	ILuint TextureName;
	ilGenImages(1, &TextureName);
	ilBindImage(TextureName);

	ILboolean success = ilLoadImage(image);
	if (success == IL_TRUE)
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
			{
				App->filesystem->SaveFile(output_file.c_str(), (char*)data, size, fileMaterial);
				App->resources->ImportFile(image, Resource_Texture);
				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &TextureName);
		}
		else
		{
			LOG("Cannot load texture from buffer of size %u", size);
		}
	}
}

void GeometryLoader::ImportImageResourceFromDragAndDrop(const char * image, std::string& output_file)
{

	ILuint TextureName;
	ilGenImages(1, &TextureName);
	ilBindImage(TextureName);

	ILboolean success = ilLoadImage(image);
	if (success == IL_TRUE)
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
			{
				App->resources->ImportFile(image, Resource_Texture);
				RELEASE_ARRAY(data);
			}
			ilDeleteImages(1, &TextureName);
		}
		else
		{
			LOG("Cannot load texture from buffer of size %u", size);
		}
	}
}


GameObject* GeometryLoader::LoadGameObject(const char* fullPath)
{
	GameObject* newObject = new GameObject();

	uint length = strlen(fullPath);

	std::string namePath = fullPath;

	uint i = namePath.find_last_of("\\");
	char* fbxname = new char[length - i + 1];
	length = length - i;
	namePath.copy(fbxname, length, i);
	fbxname[length] = '\0';
	newObject->SetName(fbxname);


	const aiScene* scene = aiImportFile(fullPath, aiProcessPreset_TargetRealtime_MaxQuality);
	if (scene != nullptr && scene->HasMeshes())
	{
		LOG("Scene %s loaded succesfully", fullPath);

		//Load transform
		aiNode* node = scene->mRootNode;
		newObject->AddComponent(LoadTransform(node));
		
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			scene->mMeshes[i]->mName = fbxname;
			scene->mMeshes[i]->mName.Append(std::to_string(i).c_str());
		}
		
		LOG("Loading meshes");

		AddGameObjectChild(node, scene, newObject);

		aiReleaseImport(scene);

		delete[] fbxname;
		fbxname = nullptr;

		return newObject;
	}
	else
	{
		LOG("Error loading scene %s", fullPath);

		delete[] fbxname;
		fbxname = nullptr;

		return nullptr;
	}
}

GameObject * GeometryLoader::AddGameObjectChild(aiNode * node, const aiScene * scene, GameObject * parent)
{
	GameObject* newparent = nullptr;
	if (node->mNumMeshes == 0)
	{
		GameObject* newObject = new GameObject(parent);

		newObject->AddComponent(LoadTransform(node));
		newObject->SetName(node->mName.C_Str());
		parent->AddChild(newObject);
		newparent = newObject;
	}
	if (node->mNumMeshes != 0)
	{
		for (uint i = 0; i < node->mNumMeshes; i++)
		{
			GameObject* newObject = new GameObject(parent);

			newObject->AddComponent(LoadTransform(node));
			newObject->SetName(node->mName.C_Str());
			parent->AddChild(newObject);
			aiMesh* mesh= scene->mMeshes[node->mMeshes[i]];
			CompMesh* m = new CompMesh();

			int meshUID = App->resources->ImportFile(mesh->mName.C_Str(), mesh);
			if (meshUID != -1)
			{
				m->AddResource(meshUID);
			}
			m->SetName("Mesh");
			m->CreateEnclosingBox();
			newObject->AddComponent(m);
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			newObject->AddComponent(LoadMaterial(material));
			App->editor->Static_Vector.push_back(newObject);
			newparent = newObject;
		}
	}
	for (uint i = 0; i < node->mNumChildren; i++)
	{
		AddGameObjectChild(node->mChildren[i], scene, newparent);
	}
	return nullptr;
}

uint GeometryLoader::ImportImage(const char * image, std::string& output_file)
{
	bool ret = false;

	ILuint TextureName;
	ilGenImages(1, &TextureName);
	ilBindImage(TextureName);

	ILboolean success = ilLoadImage(image);
	if (success == IL_TRUE)
	{
		ilEnable(IL_FILE_OVERWRITE);

		ILuint size;
		ILubyte *data;

		ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
		size = ilSaveL(IL_DDS, NULL, 0);

		if (size > 0)
		{
			data = new ILubyte[size];
			if (ilSaveL(IL_DDS, data, size) > 0)
			{
				App->filesystem->SaveFile(output_file.c_str(), (char*)data, size, fileMaterial);
				RELEASE_ARRAY(data);
				ret = true;
			}
			ilDeleteImages(1, &TextureName);
		}
		else
		{
			LOG("Cannot load texture from buffer of size %u", size);
			ret = false;
		}
	}
	return ret;
}

bool GeometryLoader::SaveTextureToOwnFormat(const char * path, char * buffer, int buffer_size)
{
	uint length = strlen(path);
	std::string namePath = path;
	uint lenght = 0;
	uint i = namePath.find_last_of("/");
	i++;
	char* testM = new char[length - i - 3];
	length = length - i;
	namePath.copy(testM, length, i);
	testM[length - 4] = '\0';
	App->filesystem->SaveFile(testM, buffer, buffer_size, fileMaterial);
	return true;
}

uint GeometryLoader::LoadMaterial(const char * image)
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

void GeometryLoader::UnloadTexture(uint id)
{
		glDeleteTextures(1, &id);
		LOG("Deleting texture with %i ID from GPU", id);
}

bool GeometryLoader::SaveMeshToOwnFormat(const aiMesh* mesh, const char * outputFile)
{

	ResourceMesh* tmpMesh = new ResourceMesh(0);

	tmpMesh->numVertices = mesh->mNumVertices;
	tmpMesh->vertices = new float[tmpMesh->numVertices * 3];
	memcpy(tmpMesh->vertices, mesh->mVertices, sizeof(float) * tmpMesh->numVertices * 3);
	LOG("Saving mesh with %d vertices", tmpMesh->numVertices);

	if (mesh->HasFaces())
	{
		tmpMesh->numIndices = mesh->mNumFaces * 3;
		tmpMesh->indices = new uint[tmpMesh->numIndices];
		for (uint i = 0; i < mesh->mNumFaces; ++i)
		{
			if (mesh->mFaces[i].mNumIndices != 3)
			{
				LOG("WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&tmpMesh->indices[i * 3], mesh->mFaces[i].mIndices, 3 * sizeof(uint));
			}
		}
	}
	if (mesh->HasNormals())
	{
		tmpMesh->normals = new float[tmpMesh->numVertices * 3];
		memcpy(tmpMesh->normals, mesh->mNormals, sizeof(float) * tmpMesh->numVertices * 3);
	}
	if (mesh->HasTextureCoords(0))
	{
		tmpMesh->texCoords = new float[tmpMesh->numVertices * 3];
		memcpy(tmpMesh->texCoords, mesh->mTextureCoords[0], sizeof(float) * tmpMesh->numVertices * 3);
	}
	//SAVING MESH TO OWN FORMAT
int ranges[4] = { tmpMesh->numIndices, tmpMesh->numVertices, tmpMesh->numVertices, tmpMesh->numVertices};
		float size = sizeof(ranges);
		size += sizeof(uint) *  tmpMesh->numIndices;
		size += sizeof(float) *  tmpMesh->numVertices * 3;
		size += sizeof(float) *  tmpMesh->numVertices * 3;
		size += sizeof(float) *  tmpMesh->numVertices * 3;

		char* data = new char[size];
		char* cursor = data;

		uint bytes = 0;
		bytes = sizeof(ranges); // First store ranges
		memcpy(cursor, ranges, bytes);
		cursor += bytes;

		// Store indices
		bytes = sizeof(uint) *  tmpMesh->numIndices;
		memcpy(cursor, tmpMesh->indices, tmpMesh->numIndices * sizeof(uint));
		cursor += bytes;

		// Store vertices
		bytes = sizeof(float) *  tmpMesh->numVertices * 3;
		memcpy(cursor, tmpMesh->vertices, tmpMesh->numVertices * 3 * sizeof(float));
		cursor += bytes;

		// Store normals
		bytes = sizeof(float) *  tmpMesh->numVertices * 3;
		memcpy(cursor, tmpMesh->normals, tmpMesh->numVertices * 3 * sizeof(float));
		cursor += bytes;

		if (tmpMesh->texCoords != nullptr)
		{
			// Store tex coords
			bytes = sizeof(float) *  tmpMesh->numVertices * 3;
			memcpy(cursor, tmpMesh->texCoords, tmpMesh->numVertices * 3 * sizeof(float));
		}
		cursor += bytes;

		App->filesystem->SaveFile(outputFile, data, size, fileMesh);

		delete tmpMesh;
		RELEASE_ARRAY(data);

		return true;

}

void GeometryLoader::LoadMeshOwnFormat(const char * inputFile, ResourceMesh * mesh)
{
	char* buffer;
	int size;

	if (App->filesystem->LoadFile(inputFile, &buffer, size, fileMesh) == true)
	{
		char* cursor = buffer;
		// amount of indices / vertices / colors / normals / texture_coords
		uint ranges[4];
		uint bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);

		mesh->numIndices = ranges[0];
		mesh->numVertices = ranges[1];

		// Load indices
		cursor += bytes;
		bytes = sizeof(uint) * mesh->numIndices;
		mesh->indices = new uint[mesh->numIndices];
		memcpy(mesh->indices, cursor, bytes);

		glGenBuffers(1, (GLuint*)&mesh->idIndices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->idIndices);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * mesh->numIndices, mesh->indices, GL_STATIC_DRAW);

		// Load vertices
		cursor += bytes;
		bytes = sizeof(float) * mesh->numVertices * 3;
		mesh->vertices = new float[mesh->numVertices * 3];
		memcpy(mesh->vertices, cursor, bytes);

		glGenBuffers(1, (GLuint*)&mesh->idVertices);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->numVertices * 3, mesh->vertices, GL_STATIC_DRAW);

		cursor += bytes;
		bytes = sizeof(float) * mesh->numVertices * 3;
		mesh->normals = new float[mesh->numVertices * 3];
		memcpy(mesh->normals, cursor, bytes);

		glGenBuffers(1, (GLuint*)&mesh->idNormals);
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->numVertices * 3, mesh->normals, GL_STATIC_DRAW);

		cursor += bytes;
		bytes = sizeof(float) * mesh->numVertices * 3;
		mesh->texCoords = new float[mesh->numVertices * 3];
		memcpy(mesh->texCoords, cursor, bytes);

		glGenBuffers(1, (GLuint*) &(mesh->idTexCoords));
		glBindBuffer(GL_ARRAY_BUFFER, mesh->idTexCoords);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh->numVertices * 3, mesh->texCoords, GL_STATIC_DRAW);

		delete[] buffer;
		buffer = nullptr;
	}

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
		std::string fullPath = "Assets/";
		uint l = fullPath.size();
		fullPath.append(lastpath);

		int texUID = App->resources->ImportFile(fullPath.c_str(), Resource_Texture);
		if (texUID != -1)
		{
			m->AddResource(texUID);
		}

		std::string atname = lastpath;
		uint r = atname.find_first_of(".");
		std::string matname = atname.substr(0,r);
		m->SetName(matname.c_str());

		delete[] lastpath;
		lastpath = nullptr;
		return m;
	}
	return nullptr;
}

CompTransform* GeometryLoader::LoadTransform(aiNode* node)
{
	aiVector3D translation = { 0,0,0 };
	aiVector3D scale = { 1,1,1 };
	aiQuaternion q1 = { 0,0,0,0 };
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

