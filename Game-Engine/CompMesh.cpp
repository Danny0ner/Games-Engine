#include "CompMesh.h"
#include "Primitive.h"
#include "Application.h"
#include "GeometryLoader.h"
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

void CompMesh::Update()
{
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
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1},"Vertices ID: %i", resourceMesh->idVertices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Vertices: %i", resourceMesh->numVertices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Indexes ID: %i", resourceMesh->idIndices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Indexes: %i", resourceMesh->numIndices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Normals ID: %i", resourceMesh->idNormals);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Colors ID: %i", resourceMesh->idColors);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Texture Coords: %i", resourceMesh->idTexCoords);
		if (ImGui::Checkbox("Debug Draw", &drawdebug)) {}
		ImGui::TreePop();
	}
}

void CompMesh::OnSave(Configuration & data) const
{
	data.SetInt("Resource UID", resourceMesh->GetUID());
}

void CompMesh::OnLoad(Configuration & data)
{
	AddResource(data.GetInt("Resource UID"));
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

void CompMesh::AddResource(int uid)
{
	resourceMesh = (ResourceMesh*)App->resources->Get(uid);
	if(resourceMesh != nullptr)
	resourceMesh->LoadToComponent();
}

