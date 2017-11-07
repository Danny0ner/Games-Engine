#include "CompTransform.h"
#include "GameObject.h"
#include "ImGui\imgui.h"
#include "Application.h"


CompTransform::CompTransform(float3 pos, float3 scale, Quat rot, ComponentType type) : Component(Component_Transform), position(pos), movedposition(pos), scale(scale), rotation(rot)
{
	name = "Transform";
	float3 eulerrot = rotation.ToEulerXYZ().Abs() * RADTODEG;
	rotation.x = eulerrot.x;
	rotation.y = eulerrot.y;
	rotation.z = eulerrot.z;
	UpdatePositionMatrix();
}

CompTransform::~CompTransform()
{
	
}

void CompTransform::Update()
{
	UpdatePositionMatrix();
}

void CompTransform::UpdatePositionMatrix()
{
	rotation.x *= DEGTORAD;
	rotation.y *= DEGTORAD;
	rotation.z *= DEGTORAD;
	rotation.w *= DEGTORAD;
	Quat rotation_euler = Quat::FromEulerXYZ(rotation.x, rotation.y, rotation.z);
	TransMatrix = float4x4::FromQuat(rotation_euler);
	rotation.x *= RADTODEG;
	rotation.y *= RADTODEG;
	rotation.z *= RADTODEG;
	rotation.w *= RADTODEG;
	TransMatrix = float4x4::Scale(scale, float3(0, 0, 0)) * TransMatrix;
	TransMatrix.float4x4::SetTranslatePart(position.x, position.y, position.z);
	if (myGO != nullptr)
	{
		GameObject* GO = myGO->GetParent();
		while (GO != nullptr)
		{
			CompTransform* transf = dynamic_cast<CompTransform*>(GO->FindComponent(Component_Transform));
			if(transf != nullptr) TransMatrix = transf->GetTransMatrix()* TransMatrix;
			GO = GO->GetParent();
		}
	}

}

void CompTransform::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Position", &position.x, 0.5, -1000,1000,"%.2f"))
		{
		}
		if (ImGui::DragFloat3("Rotation", &rotation.x, 1, -360,360, "%.2f"))
		{
		}

		if (ImGui::DragFloat3("Scale", &scale.x, 0.5, -30,30, "%.2f"))
		{
		}
		ImGui::TreePop();
	}
}

float4x4 CompTransform::GetTransMatrix()
{

	return TransMatrix;
}

void CompTransform::OnSave(Configuration & data) const
{
	data.SetInt("Type", type);
	data.AddArrayFloat("Position", &position.x, 3);
	data.AddArrayFloat("Rotation", &rotation.x, 4);
	data.AddArrayFloat("Scale", &scale.x, 3);
}

void CompTransform::OnLoad(Configuration & data)
{
	position.x = data.GetFloat("Position", 0);
	position.y = data.GetFloat("Position", 1);
	position.z = data.GetFloat("Position", 2);
	rotation.x = data.GetFloat("Rotation", 0);
	rotation.y = data.GetFloat("Rotation", 1);
	rotation.z = data.GetFloat("Rotation", 2);
	rotation.w = data.GetFloat("Rotation", 3);
	scale.x = data.GetFloat("Scale", 0);
	scale.y = data.GetFloat("Scale", 1);
	scale.z = data.GetFloat("Scale", 2);
}
