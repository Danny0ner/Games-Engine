#pragma once
#include "Module.h"
#include "Globals.h"
#include "CompCamera.h"
#include "Bullet/include/btBulletDynamicsCommon.h"
#include "MathGeo\MathGeoLib.h"
#include "MathGeo\Geometry\Frustum.h"

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
	void CenterCameraToGeometry(AABB meshAABB);
	GameObject* MousePicking(float3* HitPoint = nullptr);
	void SetRaypoints(float xx, float xy, float xz, float cx, float cy, float cz);
	void SetAspectRatio();
private:

	void CalculateViewMatrix();

public:
	math::LineSegment				mouse_picking;
	float3			X, Y, Z, Position, Reference;
	btVector3		temp;
	float3			playerpos;
	float3			CameraLocation;
	float3			ViewVector;
	int				camera_dist;
	float3			VehicleLocation;
	uint			camera_fx;
	float			FOV;
	bool			FPS;
	Frustum			FrustumPick;
private:

	float4x4			ViewMatrix, ViewMatrixInverse;
	
};