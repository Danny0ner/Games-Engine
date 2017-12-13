#include "CompBone.h"
#include "Application.h"
#include "CompMaterial.h"
#include "ImGui\imgui.h"
#include "mmgr\mmgr.h"
#include "CompTransform.h"

CompBone::CompBone(MeshBone * bone, CompMesh * meshtodeform) : Component(Component_Bone)
{
	actualbone = bone;
	MeshToDeform = meshtodeform;
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
		if (getMyGO()->childs.size() == 0)
		{
			glBindBuffer(GL_ARRAY_BUFFER, MeshToDeform->deformableMesh->idVertices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshToDeform->deformableMesh->numVertices * 3, MeshToDeform->deformableMesh->vertices, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, MeshToDeform->deformableMesh->idNormals);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float) * MeshToDeform->deformableMesh->numVertices * 3, MeshToDeform->deformableMesh->normals, GL_DYNAMIC_DRAW);
		}
	}
}

void CompBone::OnEditor()
{
	//if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	//{
	//	ImGui::Text(name.c_str());

	//	if (resourceTex != nullptr)
	//	{
	//		ImGui::Text("Texture ID: %i", resourceTex->textureID);
	//		ImGui::Text("Resource ID: %i", resourceTex->GetUID());
	//		ImGui::Text("Resource reference counting: %i", resourceTex->GetReferenceCount());
	//		ImGui::Image((ImTextureID)resourceTex->textureID, ImVec2(124, 124));
	//	}
	//	if (ImGui::BeginMenu("Change Texture", &changingtexture))
	//	{
	//		App->resources->ShowTextureResources(this);
	//		ImGui::EndMenu();
	//	}

	//	ImGui::TreePop();
	//}
}

void CompBone::OnSave(Configuration & data) const
{

}

void CompBone::OnLoad(Configuration & data)
{
}
