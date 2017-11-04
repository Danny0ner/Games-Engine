#pragma once

#include "Component.h"
#include "MathGeoLib\Math\float3.h"
#include "MathGeoLib\Math\Quat.h"
#include "MathGeoLib\Math\float4x4.h"
class CompTransform : public Component
{
public:
	CompTransform(float3 pos, float3 scale, Quat rot, ComponentType type = Component_Transform);
	~CompTransform();

	void Update();
	void UpdatePositionMatrix();
	void OnEditor() override;
	float4x4 GetTransMatrix();
private:
	float3 movedposition;
	float3 position;
	float3 scale;
	Quat rotation;
	float4x4 TransMatrix;
	

	bool needToMove;
};
