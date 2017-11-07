#include "Application.h"
#include "CompMaterial.h"
#include "ImGui\imgui.h"

CompMaterial::CompMaterial() : Component(Component_Material)
{
}

CompMaterial::~CompMaterial()
{}

void CompMaterial::OnEditor()
{
	if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::Text(name.c_str());
		ImGui::Text("Texture ID: %i", idTexture);
		ImGui::TreePop();
	}
}

void CompMaterial::OnSave(Configuration & data) const
{
	data.SetString("MaterialFile", name.c_str());
}

void CompMaterial::OnLoad(Configuration & data)
{
	name = data.GetString("MaterialFile");
	App->geometryloader->LoadTextureToOwnFormat(name.c_str(), this);
}

void CompMaterial::OverrideTexture(const char* path)
{
	//idTexture = App->textures->ImportImage(path);
}