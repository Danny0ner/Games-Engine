#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysBody3D.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	turn = acceleration = brake = 0.0f;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");
/*

	VehicleInfo car;

	// Car properties ----------------------------------------
	car.num_chassis = 5;
	car.chassis_size = new vec3[car.num_chassis];
	car.chassis_size[0].Set(2, 0.7f, 2);
	car.chassis_size[1].Set(0.3f, 1.0f, 0.3f);
	car.chassis_size[2].Set(0.3f, 1.0f, 0.3f);
	car.chassis_size[3].Set(0.3f, 1.0f, 0.3f);
	car.chassis_size[4].Set(0.3f, 1.0f, 0.3f);

	car.chassis_offset = new vec3[car.num_chassis];
	car.chassis_offset[0].Set(0, 0, 0);
	car.chassis_offset[1].Set(1.1, 0.5f, -1.1);
	car.chassis_offset[2].Set(-1.1, 0.5f, 1.1);
	car.chassis_offset[3].Set(-1.1, 0.5f, -1.1);
	car.chassis_offset[4].Set(1.1, 0.5f, 1.1);
	car.chassis_color = new vec3[car.num_chassis];
	car.chassis_color[0] = { Blue.r, Blue.g, Blue.b };
	car.chassis_color[1] = { Blue.r, Blue.g, Blue.b };
	car.chassis_color[2] = { Blue.r, Blue.g, Blue.b };
	car.chassis_color[3] = { Blue.r, Blue.g, Blue.b };
	car.chassis_color[4] = { Blue.r, Blue.g, Blue.b };

	car.mass = 200.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 50.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.6f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = 3 * 0.5f;
	float half_length = 3 * 0.5f;

	vec3 direction(0, 1, 0);
	vec3 axis(-1, 0, 0);





	car.num_wheels = 0;
	car.wheels = new Wheel[0];

	
	vehicle = App->physics->AddVehicle(car);
	vehicle->SetPos(0, 1, 0);

	/////////////////////////// helices//////////////////
	Helix1.height = 0.01f;
	Helix1.radius = 0.7;
	h1 = App->physics->AddBody(Helix1, 0.0001);
	Helix1.SetRotation(90, { 1,0,0 });

	Helix2.height = 0.01f;
	Helix2.radius = 0.7;
	h2 = App->physics->AddBody(Helix2, 0.0001);

	Helix3.height = 0.01f;
	Helix3.radius = 0.7;
	h3 = App->physics->AddBody(Helix3, 0.0001);
	Helix4.height = 0.01f;
	Helix4.radius = 0.7;
	h4 = App->physics->AddBody(Helix4, 0.0001);
	Helix1.color = White;
	Helix2.color = White;
	Helix3.color = White;
	Helix4.color = White;

	btVector3 H_u_i(0.9, 1.1f, 1.1f);
	btVector3 H_u_d(0.9, -1.1f, 1.1f);
	btVector3 H_d_i(0.9, -1.1f, -1.1f);
	btVector3 H_d_d(0.9, 1.1f, -1.1f);

	btVector3 helix2(0, 2, 0);
	btVector3 helix3(0, 1, 0);
	btVector3 helix4(1, 0, 0);

	Motor = App->physics->Add_Hinge_Constraint(*vehicle->GetRigidBody(), *h1->GetRigidBody(), helix2, H_u_i, helix3, helix4, true);
	Motor2 = App->physics->Add_Hinge_Constraint(*vehicle->GetRigidBody(), *h2->GetRigidBody(), helix2, H_u_d, helix3, helix4, true);
	Motor3 = App->physics->Add_Hinge_Constraint(*vehicle->GetRigidBody(), *h3->GetRigidBody(), helix2, H_d_i, helix3, helix4, true);
	Motor4 = App->physics->Add_Hinge_Constraint(*vehicle->GetRigidBody(), *h4->GetRigidBody(), helix2, H_d_d, helix3, helix4, true);
	Motor->setLimit(0, 0);
	Motor2->setLimit(0, 0);
	Motor3->setLimit(0, 0);
	Motor4->setLimit(0, 0);
	*/
	return true;

}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");
	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	/*
	btRigidBody* bodycito;
	bodycito = vehicle->vehicle->getRigidBody();
	turn = acceleration = brake = 0.0f;
	mat4x4 trans;
	this->vehicle->GetTransform(&trans);
	vec3 PushVectorFront;
	vec3 PushVectorLeft;
	vec3 PushVectorRight;
	vec3 PushVectorBack;
	vec3 AirFriction;
	vec3 AirFric;
	btScalar frontRotx, frontRoty, frontRotz, backRotx, backRoty, backRotz;

	
	h1->GetTransform(&Helix1.transform);
	
	Motor->enableAngularMotor(true, 10, 10);
	Helix1.Render();
	h2->GetTransform(&Helix2.transform);
	Motor2->enableAngularMotor(true, 10, 10);
	Helix2.Render();
	Motor3->enableAngularMotor(true, 10, 10);
	h3->GetTransform(&Helix3.transform);
	Helix3.Render();
	Motor4->enableAngularMotor(true, 10, 10);
	h4->GetTransform(&Helix4.transform);
	Helix4.Render();
	if (true) {

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_REPEAT)
		{
			vehicle->Push(0, 70, 0);
			Normal += 70;
		}
		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_UP)
		{
			if (Normal > 1000)
				vehicle->Push(0, -Normal / 7, 0);
			else  if (Normal > 300)  vehicle->Push(0, -Normal / 10, 0);
			else vehicle->Push(0, -Normal / 15, 0);
			Normal = 0;

		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{

			vec3 WorldPush(50, 0, 0);
			PushVectorFront.x = trans[0] * WorldPush.x + trans[1] * WorldPush.y + trans[2] * WorldPush.z;
			PushVectorFront.y = trans[4] * WorldPush.x + trans[5] * WorldPush.y + trans[6] * WorldPush.z;
			PushVectorFront.z = trans[8] * WorldPush.x + trans[9] * WorldPush.y + trans[10] * WorldPush.z;
			vehicle->Push(PushVectorFront.x, PushVectorFront.y, -PushVectorFront.z);

			NormalStr += 50;
			lastdirec = Forward;
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_UP)
		{
			if (NormalStr > 2000) vehicle->Push(-PushVectorFront.x, -PushVectorFront.y, PushVectorFront.z);
			else
				vehicle->Push(-PushVectorFront.x, -PushVectorFront.y, PushVectorFront.z);

			NormalStr = 0;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			vec3 WorldPush(-50, 0, 0);
			PushVectorBack.x = trans[0] * WorldPush.x + trans[1] * WorldPush.y + trans[2] * WorldPush.z;
			PushVectorBack.y = trans[4] * WorldPush.x + trans[5] * WorldPush.y + trans[6] * WorldPush.z;
			PushVectorBack.z = trans[8] * WorldPush.x + trans[9] * WorldPush.y + trans[10] * WorldPush.z;
			vehicle->Push(PushVectorBack.x, PushVectorBack.y, -PushVectorBack.z);

			NormalBack += 50;
			lastdirec = Backward;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP)
		{

			vehicle->Push(-PushVectorBack.x, -PushVectorBack.y, PushVectorBack.z);

			NormalBack = 0;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN)
		{

		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			vec3 WorldPush(0, 0, 50);
			PushVectorLeft.x = trans[0] * WorldPush.x + trans[1] * WorldPush.y + trans[2] * WorldPush.z;
			PushVectorLeft.y = trans[4] * WorldPush.x + trans[5] * WorldPush.y + trans[6] * WorldPush.z;
			PushVectorLeft.z = trans[8] * WorldPush.x + trans[9] * WorldPush.y + trans[10] * WorldPush.z;
			vehicle->Push(PushVectorLeft.x, PushVectorLeft.y, -PushVectorLeft.z);

			NormalRight += 50;
			lastdirec = Left;

		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_UP)
		{

			vehicle->Push(-PushVectorLeft.x, -PushVectorLeft.y, PushVectorLeft.z);

			NormalRight = 0;

		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN)
		{

		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			vec3 WorldPush(0, 0, -50);
			PushVectorRight.x = trans[0] * WorldPush.x + trans[1] * WorldPush.y + trans[2] * WorldPush.z;
			PushVectorRight.y = trans[4] * WorldPush.x + trans[5] * WorldPush.y + trans[6] * WorldPush.z;
			PushVectorRight.z = trans[8] * WorldPush.x + trans[9] * WorldPush.y + trans[10] * WorldPush.z;
			vehicle->Push(PushVectorRight.x, PushVectorRight.y, -PushVectorRight.z);


			lastdirec = Right;
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_UP)
		{
			vehicle->Push(-PushVectorRight.x, -PushVectorRight.y, PushVectorRight.z);
		}

		switch (lastdirec) {
		case nothing:
			break;

		case Forward:
			AirFric = (-13, 0, 0);
			AirFriction.x = trans[0] * AirFric.x + trans[1] * AirFric.y + trans[2] * AirFric.z;
			AirFriction.y = trans[4] * AirFric.x + trans[5] * AirFric.y + trans[6] * AirFric.z;
			AirFriction.z = trans[8] * AirFric.x + trans[9] * AirFric.y + trans[10] * AirFric.z;
			vehicle->Push(AirFriction.x, AirFriction.y, -AirFriction.z);
			break;
		case Backward:
			AirFric = (13, 0, 0);
			AirFriction.x = trans[0] * AirFric.x + trans[1] * AirFric.y + trans[2] * AirFric.z;
			AirFriction.y = trans[4] * AirFric.x + trans[5] * AirFric.y + trans[6] * AirFric.z;
			AirFriction.z = trans[8] * AirFric.x + trans[9] * AirFric.y + trans[10] * AirFric.z;
			vehicle->Push(AirFriction.x, AirFriction.y, -AirFriction.z);

			break;
		case Left:

			AirFriction.x = trans[0] * AirFric.x + trans[1] * AirFric.y + trans[2] * AirFric.z;
			AirFriction.y = trans[4] * AirFric.x + trans[5] * AirFric.y + trans[6] * AirFric.z;
			AirFriction.z = trans[8] * AirFric.x + trans[9] * AirFric.y + trans[10] * AirFric.z;
			vehicle->Push(-AirFriction.x, 0, -AirFriction.z);
			break;
		case Right:

			AirFriction.x = trans[0] * AirFric.x + trans[1] * AirFric.y + trans[2] * AirFric.z;
			AirFriction.y = trans[4] * AirFric.x + trans[5] * AirFric.y + trans[6] * AirFric.z;
			AirFriction.z = trans[8] * AirFric.x + trans[9] * AirFric.y + trans[10] * AirFric.z;
			vehicle->Push(AirFriction.x, 0, -AirFriction.z);
			break;
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		{
			bodycito->applyTorque({ 0,1000,0 });
			LeftRotation += 1000;
		}
		if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_UP)
		{
			bodycito->setAngularVelocity({ 0,0,0 });
		}

		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		{
			bodycito->applyTorque({ 0,-1000,0 });
			RightRotation += 1000;
		}
		if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_UP)
		{
			bodycito->setAngularVelocity({ 0,0,0 });
		}
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		{
			frontRotx = trans[0] * 0 + trans[1] * 0 + trans[2] * 1000;
			frontRoty = trans[4] * 0 + trans[5] * 0 + trans[6] * 1000;
			frontRotz = trans[8] * 0 + trans[9] * 0 + trans[10] * 1000;
			bodycito->applyTorque({ frontRotx,frontRoty, -frontRotz });
			FrontRotation += 1000;
		}
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_UP)
		{
			bodycito->setAngularVelocity({ 0,0,0 });
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		{
			backRotx = trans[0] * 0 + trans[1] * 0 + trans[2] * -1000;
			backRoty = trans[4] * 0 + trans[5] * 0 + trans[6] * -1000;
			backRotz = trans[8] * 0 + trans[9] * 0 + trans[10] * -1000;
			bodycito->applyTorque({ backRotx,backRoty, -backRotz });
		}
		if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_UP)
		{
			bodycito->setAngularVelocity({ 0,0,0 });
		}
		if (vehicle->GetKmh() < 10 && vehicle->GetKmh() > -10) lastdirec = nothing;
		

		if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		{
			bodycito->setAngularVelocity({ 0,0,0 });
		}

		if (App->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
		{
			bodycito->setLinearVelocity({ 0,0,0 });
			vehicle->SetPos(0, 2, 0);
		}
	}
	vehicle->Render();
	*/
	return UPDATE_CONTINUE;
}



