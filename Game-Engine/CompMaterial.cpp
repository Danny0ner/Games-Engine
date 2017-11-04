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
	if (ImGui::TreeNodeEx("Material"))
	{
		ImGui::Text(name.c_str());
		ImGui::Text("Texture ID: %i", idTexture);
		ImGui::TreePop();
	}
}

void CompMaterial::OverrideTexture(const char* path)
{
	//idTexture = App->textures->ImportImage(path);
}