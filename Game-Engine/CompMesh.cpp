#include "CompMesh.h"
#include "Primitive.h"
#include "Application.h"
#include "GeometryLoader.h"
#include "CompTransform.h"
#include "GameObject.h"
#include "ImGui\imgui.h"
#include "Glew\include\glew.h"
#include "MathGeo\Geometry\Triangle.h"
#include "MathGeo\Math\float4x4.h"

#include <vector>
CompMesh::CompMesh() : Component(Component_Mesh)
{
	name = "Mesh";
}

CompMesh::~CompMesh()
{

}

void CompMesh::Update(float dt)
{
	//for (uint i = 0; i < bones.size(); i++)
	//{

	//	float4x4 matrix = bones[i]->GetSystemTransform();
	//	matrix = gameObject->GetComponent<C_Transform>()->GetTransform().Inverted() * matrix;
	//	matrix = matrix * rBone->offset;

	//	for (uint i = 0; i < rBone->numWeights; i++)
	//	{
	//		uint index = rBone->weightsIndex[i];
	//		float3 originalV(&rMesh->vertices[index * 3]);

	//		float3 toAdd = matrix.TransformPos(originalV);

	//		animMesh->vertices[index * 3] += toAdd.x  * rBone->weights[i];
	//		animMesh->vertices[index * 3 + 1] += toAdd.y * rBone->weights[i];
	//		animMesh->vertices[index * 3 + 2] += toAdd.z * rBone->weights[i];

	//		if (rMesh->buffersSize[R_Mesh::b_normals] > 0)
	//		{
	//			toAdd = matrix.TransformPos(float3(&rMesh->normals[index * 3]));
	//			animMesh->normals[index * 3] += toAdd.x * rBone->weights[i];
	//			animMesh->normals[index * 3 + 1] += toAdd.y * rBone->weights[i];
	//			animMesh->normals[index * 3 + 2] += toAdd.z * rBone->weights[i];
	//		}
	//	}
	//}
	if (deformableMesh != nullptr)
	{
		GameObject* bone = nullptr;
		ResetDeformableMesh();
		for (int i = 0; i < resourceskeleton->MeshBones.size(); i++)
		{
			myGO->GetParent()->FindSiblingOrChildGameObjectWithName(resourceskeleton->MeshBones[i]->name.c_str(), bone);
			
			CompTransform* trans = (CompTransform*)bone->FindComponent(Component_Transform);
			CompTransform* thistransform = (CompTransform*)myGO->FindComponent(Component_Transform);

			float4x4 bonematrix =trans->GetTransMatrix();
			bonematrix = thistransform->GetLocalMatrix().Inverted() * bonematrix;
			bonematrix = bonematrix * resourceskeleton->MeshBones[i]->offsetmatrix;

			for (int x = 0; x < resourceskeleton->MeshBones[i]->VertexWeights.size(); x++)
			{
				int actualvertexpos = (resourceskeleton->MeshBones[i]->VertexWeights[x]->VertexID);

				float3 originalV(&resourceMesh->vertices[actualvertexpos*3]);
				float3 toAdd = bonematrix.TransformPos(originalV);

				deformableMesh->vertices[actualvertexpos*3] += toAdd.x * resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;
				deformableMesh->vertices[actualvertexpos*3 + 1] += toAdd.y * resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;
				deformableMesh->vertices[actualvertexpos*3 + 2] += toAdd.z * resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;

				/*float3 originalN(&resourceMesh->normals[actualvertexpos]);
				float3 toAddd = bonematrix.TransformPos(originalN);
				deformableMesh->normals[actualvertexpos] += toAddd.x * resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;;
				deformableMesh->normals[actualvertexpos+ 1] += toAddd.y * resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;;
				deformableMesh->normals[actualvertexpos+ 2] += toAddd.z *  resourceskeleton->MeshBones[i]->VertexWeights[x]->Weight;*/
			}
		}
		glBindBuffer(GL_ARRAY_BUFFER, deformableMesh->idVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * deformableMesh->numVertices * 3, deformableMesh->vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, deformableMesh->idNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * deformableMesh->numVertices * 3, deformableMesh->normals, GL_DYNAMIC_DRAW);
	}
}

float3 CompMesh::GetCenter() const
{
	return enclosingBox.Centroid();
}

void CompMesh::DrawDebug() const
{
	if (resourceMesh->idNormals > 0)
	{
		for (int i = 0; i < resourceMesh->numVertices * 3; i += 3)
		{
			pLine vNormal(resourceMesh->vertices[i], resourceMesh->vertices[i + 1], resourceMesh->vertices[i + 2], 
				resourceMesh->vertices[i] + resourceMesh->normals[i], resourceMesh->vertices[i + 1] + resourceMesh->normals[i + 1], resourceMesh->vertices[i + 2] + resourceMesh->normals[i + 2]);
			vNormal.color = {1.0f, 0.85f, 0.0f};
			vNormal.Render();
		}
	}

	//Draw enclosing box ----
	float3 corners[8];
	enclosingBox.GetCornerPoints(corners);

	//glColor3f(Azure.r, Azure.g, Azure.b);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glBegin(GL_QUADS);

	//TODO: improve this function
	glVertex3fv((GLfloat*)&corners[1]); //glVertex3f(-sx, -sy, sz);
	glVertex3fv((GLfloat*)&corners[5]); //glVertex3f( sx, -sy, sz);
	glVertex3fv((GLfloat*)&corners[7]); //glVertex3f( sx,  sy, sz);
	glVertex3fv((GLfloat*)&corners[3]); //glVertex3f(-sx,  sy, sz);

	glVertex3fv((GLfloat*)&corners[4]); //glVertex3f( sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[2]); //glVertex3f(-sx,  sy, -sz);
	glVertex3fv((GLfloat*)&corners[6]); //glVertex3f( sx,  sy, -sz);

	glVertex3fv((GLfloat*)&corners[5]); //glVertex3f(sx, -sy,  sz);
	glVertex3fv((GLfloat*)&corners[4]); //glVertex3f(sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[6]); //glVertex3f(sx,  sy, -sz);
	glVertex3fv((GLfloat*)&corners[7]); //glVertex3f(sx,  sy,  sz);

	glVertex3fv((GLfloat*)&corners[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[1]); //glVertex3f(-sx, -sy,  sz);
	glVertex3fv((GLfloat*)&corners[3]); //glVertex3f(-sx,  sy,  sz);
	glVertex3fv((GLfloat*)&corners[2]); //glVertex3f(-sx,  sy, -sz);

	glVertex3fv((GLfloat*)&corners[3]); //glVertex3f(-sx, sy,  sz);
	glVertex3fv((GLfloat*)&corners[7]); //glVertex3f( sx, sy,  sz);
	glVertex3fv((GLfloat*)&corners[6]); //glVertex3f( sx, sy, -sz);
	glVertex3fv((GLfloat*)&corners[2]); //glVertex3f(-sx, sy, -sz);

	glVertex3fv((GLfloat*)&corners[0]); //glVertex3f(-sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[4]); //glVertex3f( sx, -sy, -sz);
	glVertex3fv((GLfloat*)&corners[5]); //glVertex3f( sx, -sy,  sz);
	glVertex3fv((GLfloat*)&corners[1]); //glVertex3f(-sx, -sy,  sz);

	glEnd();


	glColor3f(1, 1, 1);
}

void CompMesh::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (resourceMesh != nullptr)
		{
			ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Vertices ID: %i", resourceMesh->idVertices);
			ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Vertices: %i", resourceMesh->numVertices);
			ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Indexes ID: %i", resourceMesh->idIndices);
			ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Indexes: %i", resourceMesh->numIndices);
			ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Resource reference counting: %i", resourceMesh->GetReferenceCount());
			if (ImGui::Checkbox("Debug Draw", &drawdebug)) {}
		}
		if(ImGui::BeginMenu("Change Mesh", &ChangingMesh))
		{
			App->resources->ShowMeshResources(this);
			ImGui::EndMenu();
		}
		ImGui::TreePop();
	}
}

void CompMesh::OnSave(Configuration & data) const
{
	if (resourceMesh != nullptr)
	{
		data.SetString("ResourceMesh Name", resourceMesh->GetFile());
	}
	else
	{
		data.SetString("ResourceMesh Name", "noresource");
	}

}

void CompMesh::OnLoad(Configuration & data)
{
	AddResourceByName(data.GetString("ResourceMesh Name"));
}

AABB CompMesh::GetEnclosingBox()
{
	return enclosingBox;
}

void CompMesh::CreateEnclosingBox()
{
	if (resourceMesh != nullptr)
	{
		enclosingBox.SetNegativeInfinity();
		enclosingBox.Enclose((float3*)resourceMesh->vertices, resourceMesh->numVertices);
	}
}

void CompMesh::AddResourceByName(std::string filename)
{
	resourceMesh = (ResourceMesh*)App->resources->GetResourceByName(filename.c_str());
	if(resourceMesh != nullptr)
	resourceMesh->LoadToComponent();
}

void CompMesh::AddResource(int uid)
{
	resourceMesh = (ResourceMesh*)App->resources->Get(uid);
	if (resourceMesh != nullptr)
		resourceMesh->LoadToComponent();
}

void CompMesh::AddResourceSkeletonByName(std::string filename)
{
	resourceskeleton = (ResourceSkeleton*)App->resources->GetResourceByName(filename.c_str());
	if (resourceskeleton != nullptr)
		resourceskeleton->LoadToComponent();
}

void CompMesh::AddResourceSkeleton(int uid)
{
	resourceskeleton = (ResourceSkeleton*)App->resources->Get(uid);
	if (resourceskeleton != nullptr)
		resourceskeleton->LoadToComponent();
}

ResourceMesh * CompMesh::GetResourceMesh()
{
	if (resourceMesh != nullptr)
		return resourceMesh;
	else
		return nullptr;
}

void CompMesh::CreateDeformableMesh()
{
	deformableMesh = new DeformableMesh();
	deformableMesh->vertices = new float[resourceMesh->numVertices * 3 * sizeof(float)];
	memcpy(deformableMesh->vertices, resourceMesh->vertices, resourceMesh->numVertices * 3 * sizeof(float));
	deformableMesh->numVertices = resourceMesh->numVertices;
	deformableMesh->normals = new float[resourceMesh->numVertices * 3 * sizeof(float)];
	memcpy(deformableMesh->normals, resourceMesh->normals, resourceMesh->numVertices * 3 * sizeof(float));
	
	glGenBuffers(1, (GLuint*)&deformableMesh->idVertices);
	glBindBuffer(GL_ARRAY_BUFFER, deformableMesh->idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * deformableMesh->numVertices * 3, deformableMesh->vertices, GL_DYNAMIC_DRAW);
	
	glGenBuffers(1, (GLuint*)&deformableMesh->idNormals);
	glBindBuffer(GL_ARRAY_BUFFER, deformableMesh->idNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * deformableMesh->numVertices * 3, deformableMesh->normals, GL_DYNAMIC_DRAW);
}

void CompMesh::ResetDeformableMesh()
{
	memcpy(deformableMesh->vertices, resourceMesh->vertices, deformableMesh->numVertices * 3 * sizeof(float));

	memcpy(deformableMesh->normals, resourceMesh->normals, deformableMesh->numVertices * 3 * sizeof(float));
}

