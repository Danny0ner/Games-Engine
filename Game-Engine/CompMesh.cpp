#include "CompMesh.h"
#include "Primitive.h"

#include "ImGui\imgui.h"
#include "Glew\include\glew.h"
#include "MathGeoLib\Geometry\Triangle.h"
#include "MathGeoLib\Math\float4x4.h"

#include <vector>
CompMesh::CompMesh() : Component(Component_Mesh)
{
	name = "Mesh";
}

CompMesh::~CompMesh()
{
	if (vertices != nullptr)
	{
		delete[] vertices;
		vertices = nullptr;
	}
	if (indices != nullptr)
	{
		delete[] indices;
		indices = nullptr;
	}
	if (normals != nullptr)
	{
		delete[] normals;
		normals = nullptr;
	}
	if (colors != nullptr)
	{
		delete[] colors;
		colors = nullptr;
	}
	if (texCoords != nullptr)
	{
		delete[] texCoords;
		texCoords = nullptr;
	}
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
	if (idNormals > 0)
	{
		for (int i = 0; i < numVertices * 3; i += 3)
		{
			pLine vNormal(vertices[i], vertices[i + 1], vertices[i + 2], vertices[i] + normals[i], vertices[i + 1] + normals[i + 1], vertices[i + 2] + normals[i + 2]);
			vNormal.color = {1.0f, 0.85f, 0.0f};
			vNormal.Render();
		}
	}

	//Draw enclosing box ----
	float3 corners[8];
	enclosingBox.GetCornerPoints(corners);

	glPushMatrix();

	glMultMatrixf((GLfloat*)float4x4::identity.Transposed().ptr());
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
	glPopMatrix();

	glColor3f(1, 1, 1);
}

void CompMesh::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str()))
	{
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1},"Vertices ID: %i", idVertices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Vertices: %i", numVertices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Indexes ID: %i", idIndices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Num Indexes: %i", numIndices);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Normals ID: %i", idNormals);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Colors ID: %i", idColors);
		ImGui::TextColored({ 1.0f, 0.85f, 0.0f ,1 }, "Texture Coords: %i", idTexCoords);
		if (ImGui::Checkbox("Debug Draw", &drawdebug)) {}
		ImGui::TreePop();
	}
}

void CompMesh::Move(float3 lastpos,float3 newPos)
{
	float3 differentialpos = newPos - lastpos;
	float* newVertices = new float[numVertices * 3];
	memcpy(newVertices,vertices, sizeof(float)* numVertices * 3);
	for (int i = 0; i <= numVertices * 3; i+=3)
	{
		newVertices[i] += differentialpos.x;
	}
	for (int i = 1; i <= numVertices * 3; i += 3)
	{
		newVertices[i] += differentialpos.y;
	}
	for (int i = 2; i <= numVertices * 3; i += 3)
	{
		newVertices[i] += differentialpos.z;
	}

	glBindBuffer(GL_ARRAY_BUFFER, idVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numVertices * 3, newVertices, GL_STATIC_DRAW);

	enclosingBox.SetNegativeInfinity();
	enclosingBox.Enclose((float3*)newVertices, numVertices);

	memcpy(vertices, newVertices, sizeof(float)* numVertices * 3);

	delete[] newVertices;
	newVertices = nullptr;
}
