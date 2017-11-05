#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "CompCamera.h"
#include "Bullet/include/btBulletDynamicsCommon.h"


class CompCamera;
class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

	void Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference = false);
	void LookAt(const float3 &Spot);
	void Move(const float3 &Movement);
	float* GetViewMatrix();
	void CenterCameraToGeometry(const AABB* meshAABB);
	void RecentreCameraToGeometry();
	GameObject* MousePicking(float3* HitPoint = nullptr) const;

private:

	void CalculateViewMatrix();

public:
	
	float3			X, Y, Z, Position, Reference;
	btVector3		temp;
	vec3			playerpos;
	vec3			CameraLocation;
	vec3			ViewVector;
	int				camera_dist;
	vec3			VehicleLocation;
	uint			camera_fx;
	bool			FPS;
private:

	float4x4			ViewMatrix, ViewMatrixInverse;
	const AABB*			LastCentreGeometry = nullptr;
	Frustum				FrustumPick;
};