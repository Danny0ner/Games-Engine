#include "Application.h"
#include "CompMaterial.h"
#include "ImGui\imgui.h"
#include "mmgr\mmgr.h"

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
		if(ImGui::BeginMenu("Change Texture", &changingtexture))
		{
			App->resources->ShowTextureResources(this);
			ImGui::EndMenu();
		}
		
		ImGui::TreePop();
	}
}

void CompMaterial::OnSave(Configuration & data) const
{
	if (resourceTex != nullptr)
	{
		data.SetString("ResourceTex Name", resourceTex->GetFile());
	}
	else
	{
		data.SetString("ResourceTex Name", "noresource");
	}
}

void CompMaterial::OnLoad(Configuration & data)
{
	std::string resname = data.GetString("ResourceTex Name");
	if(resname.length() > 0)
	AddResourceByName(resname);
}

void CompMaterial::OverrideTexture(const char* path)
{
	//idTexture = App->textures->ImportImage(path);
}

void CompMaterial::AddResourceByName(std::string filename)
{
	resourceTex = (ResourceTexture*)App->resources->GetResourceByName(filename.c_str());
	if (resourceTex != nullptr)
		resourceTex->LoadToComponent();
}

void CompMaterial::AddResource(int uid)
{
	resourceTex = (ResourceTexture*)App->resources->Get(uid);
	if (resourceTex != nullptr)
		resourceTex->LoadToComponent();
}

int CompMaterial::GetTextureID()
{
	if (resourceTex != nullptr)
	{
		return resourceTex->textureID;
	}
	else return -1;
}

ResourceTexture * CompMaterial::GetResourceTex()
{
	if (resourceTex != nullptr)
		return resourceTex;
	else
		return nullptr;
}
