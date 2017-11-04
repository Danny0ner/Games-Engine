#include "Globals.h"
#include "CompCamera.h"

#include "ImGui\imgui.h"
#include "Glew\include\glew.h"
#include "MathGeoLib\Math\MathAll.h"
#include "Color.h"
CompCamera::CompCamera(float3 pos, float3 front, float3 up, float nearPlaneDistance, float farPlaneDistance, float verticalFov, float aspectRatio, FrustumType type) : Component(Component_Camera)
{
	name = "Camera";
	this->aspectRatio = aspectRatio;
	this->aspectRatio = (float)16 / 9;
	frustum.type = type;
	frustum.pos = pos;
	frustum.front = front;
	frustum.up = up;
	frustum.nearPlaneDistance = nearPlaneDistance;
	frustum.farPlaneDistance = farPlaneDistance;
	FOV = verticalFov;
	frustum.verticalFov = DEGTORAD * FOV;
	frustum.horizontalFov = 2.f * atanf((tanf(frustum.verticalFov * 0.5f)) * (aspectRatio));

	frustum.ProjectionMatrix();

	frustumCulling = true;
}

CompCamera::~CompCamera()
{}

void CompCamera::Update()
{
	DrawDebug();
}

void CompCamera::SetPos(float3 newPos)
{
	frustum.pos = newPos;
}

void CompCamera::OnEditor()
{
	if (ImGui::TreeNodeEx(name.c_str()))
	{
		ImGui::Text("Position:");
		ImGui::SliderFloat("X", &frustum.pos.x, -500, 500);
		ImGui::SliderFloat("Y", &frustum.pos.y, -500, 500);
		ImGui::SliderFloat("Z", &frustum.pos.z, -500, 500);
		ImGui::TreePop();

		ImGui::Text("FOV:");
		if (ImGui::SliderFloat("VFOV", &FOV, 1, 115))
		{
			SetFOV();
		}
		ImGui::Checkbox("Use Culling", &frustumCulling);
	}
}

//Sets the horizontal FOV according to current vertical
void CompCamera::SetFOV()
{
	frustum.verticalFov = DEGTORAD * FOV;
	frustum.horizontalFov = 2.f * atanf((tanf(frustum.verticalFov * 0.5f)) * (aspectRatio));
}

//Sets vertical FOV to value and horizontal according to aspect ratio
void CompCamera::SetVerticalFOV(float value)
{
	frustum.verticalFov = value;
	frustum.horizontalFov = 2 * Atan(Tan(value * 0.5f) * (aspectRatio));
}

void CompCamera::DrawDebug() const
{
	frustum.DrawDebug(Orange);
}

bool CompCamera::Contains(const AABB & aabb) const
{
	return frustum.ContainsAaBox(aabb);
}

bool CompCamera::GetFrustumCulling() const
{
	return frustumCulling;
}
