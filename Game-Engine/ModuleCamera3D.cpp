#include "Globals.h"
#include "Application.h"
#include "PhysBody3D.h"
#include "ModuleCamera3D.h"
#include "ModulePlayer.h"
#include "PhysBody3D.h"
#include "mmgr\mmgr.h"
#include "CompTransform.h"
#include "CompMesh.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Camera";

	CalculateViewMatrix();

	X = float3(1.0f, 0.0f, 0.0f);
	Y = float3(0.0f, 1.0f, 0.0f);
	Z = float3(0.0f, 0.0f, 1.0f);

	Position = float3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera
	Reference = float3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera
}

ModuleCamera3D::~ModuleCamera3D()
{}

// -----------------------------------------------------------------
bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	FrustumPick.type = PerspectiveFrustum;
	FrustumPick.pos = Position;
	FrustumPick.front = -float3(Z.x, Z.y, Z.z);
	FrustumPick.up = float3(Y.x, Y.y, Y.z);
	FrustumPick.nearPlaneDistance = 0.1f;
	FrustumPick.farPlaneDistance = 1000;
	FOV = 80;
	FrustumPick.verticalFov = DEGTORAD * FOV;
	FrustumPick.horizontalFov = 2.f * atanf((tanf(FrustumPick.verticalFov * 0.5f)) * (App->window->GetAspectRatio()));

	FrustumPick.ProjectionMatrix();
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
	if (App->editor->IsInputLocked() == true)	
 	return UPDATE_CONTINUE;

	float3 New_Position(0, 0, 0);
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

	if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT&& App->input->GetKey(SDL_SCANCODE_LALT))
	{
		Reference = Position;
		int dx = -App->input->GetMouseXMotion();
		int dy = -App->input->GetMouseYMotion();

		float Sensitivity = 0.25f;

		Position -= Reference;

		if (dx != 0)
		{
			float DeltaX = (float)dx * Sensitivity;
			Quat Rotation;
			Rotation.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX*DEGTORAD); //changed from glmath to mathgeolib//vec3 to float3//
			X = Rotation.Transform(X);
			Y = Rotation.Transform(Y);
			Z = Rotation.Transform(Z);
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;
			Quat Rotation;
			Rotation.SetFromAxisAngle(X, DeltaY*DEGTORAD);//changed from glmath to mathgeolib//vec3 to float3//
			Y = Rotation.Transform(Y);

			Z = Rotation.Transform(Z);

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = X.Cross(Z);
			}
		}

		Position = Reference + Z * (Position).Length();
	

		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)
			speed = 15 * dt;

		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT) New_Position.y += speed;
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT) New_Position.y -= speed;

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			New_Position -= Z * speed;
		}
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
			Quat Rotation;
			Rotation.SetFromAxisAngle(float3(0.0f, 1.0f, 0.0f), DeltaX*DEGTORAD); //changed from glmath to mathgeolib//vec3 to float3//
			X = Rotation.Transform(X);
			Y = Rotation.Transform(Y);
			Z = Rotation.Transform(Z);
		}

		if (dy != 0)
		{
			float DeltaY = (float)dy * Sensitivity;
			Quat Rotation;
			Rotation.SetFromAxisAngle(X, DeltaY*DEGTORAD);//changed from glmath to mathgeolib//vec3 to float3//
			Y = Rotation.Transform(Y);

			Z = Rotation.Transform(Z);

			if (Y.y < 0.0f)
			{
				Z = float3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
				Y = X.Cross(Z);
			}
		}

		Position = Reference + Z * (Position).Length();
	}

	FrustumPick.pos = Position;
	FrustumPick.front = -float3(Z.x, Z.y, Z.z);
	FrustumPick.up = float3(Y.x, Y.y, Y.z);

	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
	
		GameObject* ObjectPick = MousePicking();
		App->editor->SelectGameObject(ObjectPick);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		if (App->editor->selected != nullptr)
		{	
			CompMesh* tmpMesh = (CompMesh*)App->editor->selected->FindComponent(Component_Mesh);
			CompTransform* tmpTrans = (CompTransform*)App->editor->selected->FindComponent(Component_Transform);
			AABB	tempAABB = tmpMesh->enclosingBox;
			tempAABB.TransformAsAABB(tmpTrans->GetTransMatrix());
			CenterCameraToGeometry(tempAABB);
		}
	}
	
	// Recalculate matrix -------------
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
void ModuleCamera3D::Look(const float3 &Position, const float3 &Reference, bool RotateAroundReference)
{
	this->Position = Position;
	this->Reference = Reference;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	if (!RotateAroundReference)
	{
		this->Reference = this->Position;
		this->Position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const float3 &Spot)
{
	Reference = Spot;

	Z = (Position - Reference).Normalized();
	X = (float3(0.0f, 1.0f, 0.0f).Cross(Z)).Normalized();
	Y = Z.Cross(X);

	CalculateViewMatrix();
}


// -----------------------------------------------------------------
void ModuleCamera3D::Move(const float3 &Movement)
{
	Position += Movement;
	Reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return (float*)ViewMatrix.v;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = float4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -X.Dot(Position), -Y.Dot(Position), -Z.Dot(Position), 1.0f);
	ViewMatrixInverse = ViewMatrix.Inverted();


}

void ModuleCamera3D::CenterCameraToGeometry( AABB meshAABB)
{
	
		Reference = float3(0.0f, 2.0f, 0.0f);//2.0f to better look on camera

		float3 centre = meshAABB.CenterPoint();
		float3 newpos = meshAABB.Size()*0.65;
		Position = float3(centre.x + newpos.x, centre.y + newpos.y, centre.z + newpos.z + 5);
		Reference = float3(centre.x + newpos.x, centre.y + newpos.y, centre.z + newpos.z);
		LookAt(float3(centre.x, centre.y, centre.z));
	}



GameObject*  ModuleCamera3D::MousePicking(float3* HitPoint)
{
	float distance=0;
	float2 MousePos;
	GameObject* Hit;

	float width = (float)App->window->GetWidth();
	float height = (float)App->window->GetHeight();

	int mouse_x, mouse_y;
	App->input->GetMousePosition(mouse_x, mouse_y);
	
	float normalized_x = -(1.0f - (float(mouse_x / width) * 2.0f));
	float normalized_y = 1.0f - (float(mouse_y/ height) * 2.0f);

	mouse_picking = FrustumPick.UnProjectLineSegment(normalized_x, normalized_y);

	Hit = App->editor->CastRay(mouse_picking, distance);
	if (HitPoint != nullptr && Hit != nullptr)
	{
		HitPoint = &mouse_picking.GetPoint(distance);
	}
	return Hit;
}

void ModuleCamera3D::SetRaypoints(float xx, float xy, float xz, float cx, float cy, float cz)
{
}

void ModuleCamera3D::SetAspectRatio()
{
	FrustumPick.verticalFov = DEGTORAD * FOV;
	FrustumPick.horizontalFov = 2.f * atanf((tanf(FrustumPick.verticalFov * 0.5f)) * (App->window->GetAspectRatio()));
}
