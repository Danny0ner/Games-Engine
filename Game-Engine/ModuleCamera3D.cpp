#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModulePlayer.h"
#include "PhysBody3D.h"
#include "mmgr\mmgr.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";

	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	Position = vec3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera
	Reference = vec3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;
	/*CameraLocation = vec3(0.0f, 15, 0.0f);
	ViewVector = vec3(0.0f, 10.05f, 0.0f);
	camera_dist = 27;*/
	return ret;
}

// -----------------------------------------------------------------
bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

// -----------------------------------------------------------------
update_status ModuleCamera3D::Update(float dt)
{

	vec3 New_Position(0, 0, 0);
	float speed = 5.0f*dt;

	bool zoomed = false;
	if (App->input->GetMouseZ() != 0)
	{
		New_Position -= App->input->GetMouseZ()*Z*speed*2;// 2 for more speed on zoom
		zoomed = true;

	}
	Position += New_Position;

	if (!zoomed)
		Reference += New_Position;

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT) 
	{
		Reference = Position;
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 15 * dt;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) New_Position.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) New_Position.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT) New_Position -= Z * speed;
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) New_Position += Z * speed;


		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) New_Position -= X * speed;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) New_Position += X * speed;

		Position += New_Position;
		Reference += New_Position;
	}
	// Mouse motion ----------------

	if (App->input->GetKey(SDL_SCANCODE_LALT)== KEY_REPEAT) //App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT &&// 
	{

		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;

			X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;

			Y = rotate(Y, DeltaY, X);
			Z = rotate(Z, DeltaY, X);

			if (Y.y < 0.0f)
			{
				Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = cross(Z, X);
			}
		}

		Position = Reference + Z * length(Position);
	}

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		GameObject* ObjectPick = MousePicking();
		if (ObjectPick != nullptr)
			App->editor->SelectGameObject(ObjectPick);



	}
	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
		RecentreCameraToGeometry();
	
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	Reference = Spot;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, Position), -dot(Y, Position), -dot(Z, Position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);


}

void ModuleCamera3D::CenterCameraToGeometry(const AABB* meshAABB)
{
	if (meshAABB == nullptr)
		Reference = vec3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera
	else
	{
		vec centre = meshAABB->CenterPoint();
		float3 newpos = meshAABB->Size()*0.65;
		Position = vec3(centre.x + newpos.x, centre.y + newpos.y, centre.z + newpos.z + 5);
		Reference = vec3(centre.x + newpos.x, centre.y + newpos.y, centre.z + newpos.z);
		LookAt(vec3(centre.x, centre.y, centre.z));
		LastCentreGeometry = meshAABB;
	}
}

void ModuleCamera3D::RecentreCameraToGeometry()
{
	CenterCameraToGeometry(LastCentreGeometry);

}

GameObject*  ModuleCamera3D::MousePicking(float3* HitPoint) const
{
	float height = App->window->Window_Height;
	float width = App->window->Window_Height;
	float distance=0;
	float2 MousePos;
	GameObject* Hit;

	MousePos.x = App->input->GetMouseX();		//we get the mouse position//
	MousePos.y = App->input->GetMouseY();

	float NormalX = -(1.0f - (MousePos.x*2.0f) / width);		//normalize the vector//
	float NormalY = 1.0f - (MousePos.y*2.0f) / height;

	CompCamera* Cam = (CompCamera*)App->editor->GetRoot()->FindComponent(Component_Camera);
	LineSegment Picking =  Cam->frustum.UnProjectLineSegment(NormalX, NormalY);

	Hit = App->editor->CastRay(Picking, distance);
	if (HitPoint != nullptr && Hit != nullptr)
	{
		HitPoint = &Picking.GetPoint(distance);
	}
	return Hit;
}