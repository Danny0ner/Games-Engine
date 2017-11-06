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
	if (ImGui::TreeNodeEx(name.c_str()))
	{
		ImGui::Text("Position:");
		if (ImGui::SliderFloat("X", &position.x, -200, 200))
		{
		}
		if (ImGui::SliderFloat("Y", &position.y, -100, 100))
		{
		}
		if (ImGui::SliderFloat("Z", &position.z, -200, 200))
		{
		}
		if (ImGui::SliderFloat("rotationX", &rotation.x, 0, 360))
		{
		}
		if (ImGui::SliderFloat("rotationY", &rotation.y, 0, 360))
		{
		}
		if (ImGui::SliderFloat("rotationZ", &rotation.z, 0, 360))
		{
		}
		if (ImGui::SliderFloat("scaleX", &scale.x, 0, 30))
		{
		}
		if (ImGui::SliderFloat("scaleY", &scale.y, 0, 30))
		{
		}
		if (ImGui::SliderFloat("scaleZ", &scale.z, 0, 30))
		{
		}

		ImGui::TextColored({0.7f, 1.0f, 1.0f,1.0f},"Scale: X=%.2f, Y=%.2f, Z=%.2f", scale.x, scale.y, scale.z);
		ImGui::TextColored({ 0.7f, 1.0f, 1.0f ,1.0f}, "Rotation: X=%.2f, Y=%.2f, Z=%.2f, W=%.2f", rotation.x, rotation.y, rotation.z, rotation.w);
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
