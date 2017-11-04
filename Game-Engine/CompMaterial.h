#pragma once

#include "Component.h"

class CompMaterial : public Component
{
public:
	CompMaterial();
	~CompMaterial();

	uint idTexture;
	
	void OnEditor() override;

	void OverrideTexture(const char* path);
};