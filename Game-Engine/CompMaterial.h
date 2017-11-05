#pragma once

#include "Component.h"

class CompMaterial : public Component
{
public:
	CompMaterial();
	~CompMaterial();

	uint idTexture;
	
	void OnEditor() override;
	void OnSave(Configuration& data) const;
	void OnLoad(Configuration & data);
	void OverrideTexture(const char* path);
};