#include "CompTransform.h"
#include "GameObject.h"
#include "ImGui\imgui.h"
#include "ImGui\ImGuizmo.h"
#include "Application.h"


CompTransform::CompTransform(float3 pos, float3 scale, Quat rot, ComponentType type) : Component(Component_Transform), position(pos), movedposition(pos), scale(scale), rotation(rot)
{
	name = "Transform";
	needToUpdate = false;
	eulerrot = rot.ToEulerXYZ();
	rotation = rot;
	TransMatrix = float4x4::FromQuat(rot);
	TransMatrix = float4x4::Scale(scale, float3(0, 0, 0)) * TransMatrix;
	TransMatrix.float4x4::SetTranslatePart(pos.x, position.y, position.z);
}

CompTransform::~CompTransform()
{
	
}

void CompTransform::Update()
{
	if (needToUpdate) {
		UpdatePositionMatrix();
	}
	if (myGO->selected == true)
			Guizmo(App->camera->FrustumPick);
}

void CompTransform::UpdatePositionMatrix()
{
	eulerrot.x *= DEGTORAD;
	eulerrot.y *= DEGTORAD;
	eulerrot.z *= DEGTORAD;
	rotation = Quat::FromEulerXYZ(eulerrot.x, eulerrot.y, eulerrot.z);
	TransMatrix = float4x4::FromQuat(rotation);
	TransMatrix = float4x4::Scale(scale, float3(0, 0, 0));
	TransMatrix.float4x4::SetTranslatePart(position.x, position.y, position.z);
	eulerrot.x *= RADTODEG;
	eulerrot.y *= RADTODEG;
	eulerrot.z *= RADTODEG;
	if (myGO != nullptr)
	{
		GameObject* GO = myGO->GetParent();
		while (GO != nullptr)
		{
			CompTransform* transf = dynamic_cast<CompTransform*>(GO->FindComponent(Component_Transform));
			if(transf != nullptr) TransMatrix =transf->GetTransMatrix() * TransMatrix;
			GO = GO->GetParent();
		}
	}
	needToUpdate = false;
}

void CompTransform::OnEditor()
{
	if (myGO->Static)
	{




	}
	if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::DragFloat3("Position", &position.x, 0.5, -1000,1000,"%.2f"))
		{
			needToUpdate = true;
		}
		if (ImGui::DragFloat3("Rotation", &eulerrot.x, 1, -360,360, "%.2f"))
		{
			needToUpdate = true;
		}

		if (ImGui::DragFloat3("Scale", &scale.x, 0.5, -30,30, "%.2f"))
		{
			needToUpdate = true;
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
	needToUpdate = true;
	position.x = data.GetFloat("Position", 0);
	position.y = data.GetFloat("Position", 1);
	position.z = data.GetFloat("Position", 2);
	rotation.x = data.GetFloat("Rotation", 0);
	rotation.y = data.GetFloat("Rotation", 1);
	rotation.z = data.GetFloat("Rotation", 2);
	rotation.w = data.GetFloat("Rotation", 3);
	eulerrot = rotation.ToEulerXYZ();
	scale.x = data.GetFloat("Scale", 0);
	scale.y = data.GetFloat("Scale", 1);
	scale.z = data.GetFloat("Scale", 2);
}

void CompTransform::Guizmo(Frustum& Frustum)
{
	float4x4 ViewMatrix = Frustum.ViewMatrix();
	if (ImGuizmo::IsOver())
			App->editor->LockImput();
		else
			App->editor->UnlockImput();

	float* GuizmoTransMatrix = TransMatrix.Transposed().ptr();
	float4x4 identity;
	float* GuizmoProjMatrix = Frustum.ViewProjMatrix().Transposed().ptr();

	ImGuizmo::Enable(true);
	ImGuizmo::BeginFrame();
	
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		mCurrentGizmoMode = ImGuizmo::WORLD;
	}

	if (App->input->GetKey(SDL_SCANCODE_2) == KEY_DOWN)
	{
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
		mCurrentGizmoMode = ImGuizmo::LOCAL;
	}

	if (App->input->GetKey(SDL_SCANCODE_3) == KEY_DOWN)
	{
		mCurrentGizmoOperation = ImGuizmo::SCALE;
		mCurrentGizmoMode = ImGuizmo::LOCAL;

	}

	
	
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0,0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(identity.identity.ptr(),GuizmoProjMatrix, mCurrentGizmoOperation, mCurrentGizmoMode, GuizmoTransMatrix);
	if (ImGuizmo::IsUsing())
	{
		ImGuizmo::DecomposeMatrixToComponents(GuizmoTransMatrix, (float*)position.ptr(), (float*)eulerrot.ptr(), (float*)scale.ptr());
		TransMatrix.Transpose();
		ImGuizmo::RecomposeMatrixFromComponents((float*)position.ptr(), (float*)eulerrot.ptr(), (float*)scale.ptr(), TransMatrix.ptr());
		TransMatrix.Transpose();
	}
}
