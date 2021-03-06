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
#include "CompBone.h"
#include "mmgr\mmgr.h"

#include <vector>
CompMesh::CompMesh() : Component(Component_Mesh)
{
	name = "Mesh";
}

CompMesh::~CompMesh()
{
	if (deformableMesh != nullptr)
	{
		if (deformableMesh->vertices != nullptr)
		{
			delete[] deformableMesh->vertices;
			deformableMesh->vertices = nullptr;
		}
		if (deformableMesh->normals != nullptr)
		{
			delete[] deformableMesh->normals;
			deformableMesh->normals = nullptr;
		}
		delete deformableMesh;
		deformableMesh = nullptr;
	}
}

void CompMesh::Update(float dt)
{
	
	if (App->GetGameStatus() == GameStatus::STOP)
	{
		if (deformableMesh != nullptr)
		{
			CompTransform* trans = (CompTransform*)myGO->FindComponent(Component_Transform);
			float3 parentscale = myGO->GetParent()->GetScale();
			trans->SetScale(parentscale);									//FBX and DAE files export their bones in a different scale so we need to match the mesh with the rig scaling it. 
			DeleteDeformableMesh();
			CreateEnclosingBox();
		}
	}
	if (App->GetGameStatus() == GameStatus::PLAY)
	{
		if (resourceskeleton != nullptr)
		{
			if (bonesplaceds == false)
			{
				PlaceBones();
				bonesplaceds = true;
			}

			if (deformableMesh != nullptr)
			{
				CreateDeformableMeshEnclosingBox();
				ResetDeformableMesh();
				//Used to update the deformablemesh after moving/animating it
			}
			else
			{
				CreateDeformableMesh();
				CreateDeformableMeshEnclosingBox();
			}
		}
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
		if (deformableMesh == nullptr)
		{
			for (int i = 0; i < resourceMesh->numVertices * 3; i += 3)
			{
				pLine vNormal(resourceMesh->vertices[i], resourceMesh->vertices[i + 1], resourceMesh->vertices[i + 2],
					resourceMesh->vertices[i] + resourceMesh->normals[i], resourceMesh->vertices[i + 1] + resourceMesh->normals[i + 1], resourceMesh->vertices[i + 2] + resourceMesh->normals[i + 2]);
				vNormal.color = { 1.0f, 0.85f, 0.0f };
				vNormal.Render();
			}
		}
		else
		{
			for (int i = 0; i < deformableMesh->numVertices * 3; i += 3)
			{
				pLine vNormal(deformableMesh->vertices[i], deformableMesh->vertices[i + 1], deformableMesh->vertices[i + 2],
					deformableMesh->vertices[i] + deformableMesh->normals[i], deformableMesh->vertices[i + 1] + deformableMesh->normals[i + 1], deformableMesh->vertices[i + 2] + deformableMesh->normals[i + 2]);
				vNormal.color = { 1.0f, 0.85f, 0.0f };
				vNormal.Render();
			}
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
	if (resourceskeleton != nullptr)
	{
		data.SetString("ResourceSkeleton Name", resourceskeleton->GetFile());
	}
	else
	{
		data.SetString("ResourceSkeleton Name", "noresource");
	}

}

void CompMesh::OnLoad(Configuration & data)
{
	AddResourceByName(data.GetString("ResourceMesh Name"));

	AddResourceSkeletonByName(data.GetString("ResourceSkeleton Name"));
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

void CompMesh::CreateDeformableMeshEnclosingBox()
{
	if (resourceMesh != nullptr && deformableMesh != nullptr)
	{
		enclosingBox.SetNegativeInfinity();
		enclosingBox.Enclose((float3*)deformableMesh->vertices, deformableMesh->numVertices);
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
	{
		resourceskeleton->LoadToComponent();
		CreateDeformableMesh();
	}
}

void CompMesh::AddResourceSkeleton(int uid)
{
	resourceskeleton = (ResourceSkeleton*)App->resources->Get(uid);
	if (resourceskeleton != nullptr)
	{
		resourceskeleton->LoadToComponent();
		CreateDeformableMesh();
	}
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
	if (deformableMesh == nullptr)
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

		if (myGO != nullptr)
		{
			CompTransform* trans = (CompTransform*)myGO->FindComponent(Component_Transform);
			trans->SetScale(float3(0.01f, 0.01f, 0.01f));									//FBX and DAE files export their bones in a different scale so we need to match the mesh with the rig scaling it. 
		}
	}
}

void CompMesh::ResetDeformableMesh()
{
	memcpy(deformableMesh->vertices, resourceMesh->vertices, deformableMesh->numVertices * 3 * sizeof(float));

	memcpy(deformableMesh->normals, resourceMesh->normals, deformableMesh->numVertices * 3 * sizeof(float));
}

void CompMesh::DeleteDeformableMesh()
{
	if (deformableMesh->vertices != nullptr)
	{
		delete[] deformableMesh->vertices;
		deformableMesh->vertices = nullptr;
	}
	if (deformableMesh->normals != nullptr)
	{
		delete[] deformableMesh->normals;
		deformableMesh->normals = nullptr;
	}
	delete deformableMesh;
	deformableMesh = nullptr;
}

void CompMesh::PlaceBones()
{


	GameObject* bone = nullptr;
	for (int i = 0; i < resourceskeleton->MeshBones.size(); i++)
	{
		myGO->GetParent()->FindSiblingOrChildGameObjectWithName(resourceskeleton->MeshBones[i]->name.c_str(), bone);
		CompBone* tempbone = new CompBone(resourceskeleton->MeshBones[i]);
		tempbone->SetMeshToDeform(this);
		bone->AddComponent(tempbone);
	}
}

