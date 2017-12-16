#include "CompBone.h"
#include "Application.h"
#include "CompMaterial.h"
#include "ImGui\imgui.h"
#include "mmgr\mmgr.h"
#include "CompTransform.h"

CompBone::CompBone(MeshBone * bone) : Component(Component_Bone)
{
	actualbone = bone;
}

CompBone::~CompBone()
{
	
}

void CompBone::Update(float dt)
{
	if (actualbone != nullptr && MeshToDeform != nullptr)
	{
		CompTransform* trans = (CompTransform*)myGO->FindComponent(Component_Transform);

		CompTransform* meshtransform = (CompTransform*)MeshToDeform->getMyGO()->FindComponent(Component_Transform);

		float4x4 bonematrix = trans->GetTransMatrix();

		bonematrix = meshtransform->GetLocalMatrix().Inverted() * bonematrix;

		bonematrix = bonematrix * actualbone->offsetmatrix;

		for (int x = 0; x < actualbone->VertexWeights.size(); x++)
		{
			int actualvertexpos = (actualbone->VertexWeights[x]->VertexID);

			float3 originalV(&MeshToDeform->resourceMesh->vertices[actualvertexpos * 3]);

			float3 toAdd = bonematrix.TransformPos(originalV);

			MeshToDeform->deformableMesh->vertices[actualvertexpos * 3] += toAdd.x * actualbone->VertexWeights[x]->Weight;
			MeshToDeform->deformableMesh->vertices[actualvertexpos * 3 + 1] += toAdd.y * actualbone->VertexWeights[x]->Weight;
			MeshToDeform->deformableMesh->vertices[actualvertexpos * 3 + 2] += toAdd.z * actualbone->VertexWeights[x]->Weight;

			float3 originalN(&MeshToDeform->resourceMesh->normals[actualvertexpos * 3]);

			float3 toAddd = bonematrix.TransformPos(originalN);

			MeshToDeform->deformableMesh->normals[actualvertexpos * 3] += toAddd.x * actualbone->VertexWeights[x]->Weight;;
			MeshToDeform->deformableMesh->normals[actualvertexpos * 3 + 1] += toAddd.y * actualbone->VertexWeights[x]->Weight;;
			MeshToDeform->deformableMesh->normals[actualvertexpos * 3 + 2] += toAddd.z *  actualbone->VertexWeights[x]->Weight;

		}

		glBindBuffer(GL_ARRAY_BUFFER, MeshToDeform->deformableMesh->idVertices);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshToDeform->deformableMesh->numVertices * 3, MeshToDeform->deformableMesh->vertices, GL_DYNAMIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, MeshToDeform->deformableMesh->idNormals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshToDeform->deformableMesh->numVertices * 3, MeshToDeform->deformableMesh->normals, GL_DYNAMIC_DRAW);

	}
	if (drawdebug)
	{
		DebugDraw();
	}
}

void CompBone::OnEditor()
{
	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (int i = 0; i < actualbone->VertexWeights.size(); i++)
		{
			ImGui::Text("vertex ID %i , Bone Weight %f", actualbone->VertexWeights[i]->VertexID, actualbone->VertexWeights[i]->Weight);
		}
		ImGui::Checkbox("Debugdraw", &drawdebug);

	ImGui::TreePop();
	}
}

void CompBone::OnSave(Configuration & data) const
{

}

void CompBone::OnLoad(Configuration & data)
{
}

void CompBone::DebugDraw() const
{
	CompTransform* tmpTrans = (CompTransform*)MeshToDeform->getMyGO()->FindComponent(Component_Transform);
	glPushMatrix();
	if (tmpTrans != nullptr)
	{
		glMultMatrixf(tmpTrans->GetTransMatrix().Transposed().ptr());
	}
	for (int i = 0; i < actualbone->VertexWeights.size(); i++)
	{
		glColor3f(actualbone->VertexWeights[i]->Weight, 1.0f - actualbone->VertexWeights[i]->Weight, 0.0f);
		int ind = actualbone->VertexWeights[i]->VertexID;
		float3 vertex(&MeshToDeform->deformableMesh->vertices[ind * 3]);
		glPointSize(5.0f);
		glBegin(GL_POINTS);
		glVertex3f(vertex.x, vertex.y, vertex.z);
		glEnd();
	}
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glPopMatrix();
}

void CompBone::SetMeshToDeform(CompMesh * mesh)
{
	MeshToDeform = mesh;
}
