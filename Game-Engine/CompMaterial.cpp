#include "Application.h"
#include "CompMaterial.h"
#include "ImGui\imgui.h"


CompMaterial::CompMaterial() : Component(Component_Material)
{
}

CompMaterial::~CompMaterial()
{
	if (resourceTex != nullptr)
	{
		resourceTex->UnloadFromComponent();
	}
}

void CompMaterial::OnEditor()
{
	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text(name.c_str());

		if (resourceTex != nullptr)
		{
			ImGui::Text("Texture ID: %i", resourceTex->textureID);
			ImGui::Text("Resource ID: %i", resourceTex->GetUID());
			ImGui::Text("Resource reference counting: %i", resourceTex->GetReferenceCount());
			ImGui::Image((ImTextureID)resourceTex->textureID, ImVec2(124, 124));
		}

		ImGui::TreePop();
	}
}

void CompMaterial::OnSave(Configuration & data) const
{
	if (resourceTex != nullptr)
	{
		data.SetInt("Texture UID", resourceTex->GetUID());
	}
}

void CompMaterial::OnLoad(Configuration & data)
{
	AddResource(data.GetInt("Texture UID"));
}

void CompMaterial::OverrideTexture(const char* path)
{
	//idTexture = App->textures->ImportImage(path);
}

void CompMaterial::AddResource(int uid)
{
	resourceTex = (ResourceTexture*)App->resources->Get(uid);
	if (resourceTex != nullptr)
	{
		resourceTex->LoadToComponent();
	}
}

int CompMaterial::GetTextureID()
{
	if (resourceTex != nullptr)
	{
		return resourceTex->textureID;
	}
	else return -1;
}