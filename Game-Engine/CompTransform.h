#pragma once

#include "Component.h"
#include "MathGeo\Math\float3.h"
#include "MathGeo\Math\Quat.h"
#include "MathGeo\Math\float4x4.h"
class CompTransform : public Component
{
public:
	CompTransform(float3 pos = { 0,0,0 }, float3 scale = { 1,1,1 }, Quat rot = { 0, 0, 0, 1 }, ComponentType type = Component_Transform);
	~CompTransform();

	void Update();
	void UpdatePositionMatrix();
	void OnEditor() override;
	float4x4 GetTransMatrix();
	void OnSave(Configuration& data) const;
	void OnLoad(Configuration& data);
	void Guizmo(Frustum& Camera);
	void UpdateChildsTransMatrix();

	void UpdateChildsTransMatrixNow();

	bool needToUpdate;
private:
	float3 movedposition;
	float3 position;
	float3 scale;
	Quat rotation;
	float4x4 TransMatrix;
	float3 eulerrot;
	bool showinspector = true;
};
