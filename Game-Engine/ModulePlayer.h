#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

struct PhysVehicle3D;
struct PhysVehicle3D;
enum lastdir {
	nothing,
	Forward,
	Backward,
	Left,
	Right,
};
#define MAX_ACCELERATION 0.0f
#define TURN_DEGREES 15.0f * DEGTORAD
#define BRAKE_POWER 1000.0f

class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update(float dt);
	bool CleanUp();

public:
	int Normal = 0;
	int NormalStr = 0;
	int NormalLeft = 0;
	int NormalRight = 0;
	int NormalBack = 0;

	btScalar LeftRotation = 0;
	btScalar RightRotation = 0;
	btScalar FrontRotation = 0;
	btScalar BackRotation = 0;
	float matrix[12];
	lastdir lastdirec = nothing;
	PhysVehicle3D* vehicle;
	float turn;
	float acceleration;
	float brake;

	PCylinder Helix1;
	PCylinder Helix2;
	PCylinder Helix3;
	PCylinder Helix4;

	PhysBody3D* h1;
	PhysBody3D* h2;
	PhysBody3D* h3;
	PhysBody3D* h4;
	btHingeConstraint* Motor;
	btHingeConstraint* Motor2;
	btHingeConstraint* Motor3;
	btHingeConstraint* Motor4;
};