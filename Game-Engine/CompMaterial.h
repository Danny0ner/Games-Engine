#pragma once

#include "Component.h"
#include "ResourceTexture.h"
class CompMaterial : public Component
{
public:
	CompMaterial();
	~CompMaterial();
	
	void OnEditor() override;
	void OnSave(Configuration& data) const;
	void OnLoad(Configuration & data);

	void AddResourceByName(std::string filename);
	void AddResource(int uid);
	int GetTextureID();
	ResourceTexture* GetResourceTex();
private:
	ResourceTexture* resourceTex = nullptr;
	bool changingtexture = false;
};