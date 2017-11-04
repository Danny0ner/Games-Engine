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

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	float* GetViewMatrix();
	void CenterCameraToGeometry(const AABB* meshAABB);
	void RecentreCameraToGeometry();
	GameObject* MousePicking(float3* HitPoint = nullptr) const;

private:

	void CalculateViewMatrix();

public:
	
	vec3 X, Y, Z, Position, Reference;
	btVector3 temp;
	vec3 playerpos;
	vec3			CameraLocation;
	vec3			ViewVector;
	int				camera_dist;
	vec3			VehicleLocation;
	uint			camera_fx;
	bool			FPS;
private:

	mat4x4 ViewMatrix, ViewMatrixInverse;
	const AABB*		LastCentreGeometry = nullptr;
};