#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "GeometryLoader.h"
#include <vector>
#include "mmgr\mmgr.h"
#include "CompMesh.h"
#include "CompMaterial.h"
#include "CompTransform.h"
#include "CompCamera.h"

#pragma comment (lib, "Glew/libx86/glew32.lib")  /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib")
/* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Renderer 3D";
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{
	RELEASE(grid);
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	grid = new PPlane(0,0,0,10);

	LOG("Creating 3D Renderer context");
	App->Console.AddLog("Creating 3D Renderer context");
	bool ret = true;


	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	GLenum err = glewInit();


	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if (context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		App->Console.AddLog("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	if (ret == true)
	{
		//Use Vsync
		if (VSYNC && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			App->Console.AddLog("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			App->Console.AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			App->Console.AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);

		//Initialize clear color
		glClearColor(0.14f, 0.41f, 0.75f, 1);

		//Check for error
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			App->Console.AddLog("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		GLfloat LightModelAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);

		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();

		GLfloat MaterialAmbient[] = { 1.0f, 0.5f, 0.0f, 0.7f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = { 1.0f, 0.5f, 0.0f, 0.7f };
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
	}

	OnResize(App->window->Window_Width, App->window->Window_Height);
	// Projection matrix for

	return ret;
}

bool ModuleRenderer3D::Start()
{
	
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->FrustumPick.ViewProjMatrix().Transposed().ptr());

	glMatrixMode(GL_MODELVIEW);
	glClearColor(BackgroundColor.x, BackgroundColor.y, BackgroundColor.z, 1);

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	if (drawdebug)
	{
		glBegin(GL_LINES);

		glVertex3f(App->camera->mouse_picking.a.x, App->camera->mouse_picking.a.y, App->camera->mouse_picking.a.z); glVertex3f(App->camera->mouse_picking.b.x, App->camera->mouse_picking.b.y, App->camera->mouse_picking.b.z);

		glEnd();

		App->editor->GetQuadtree()->DrawDebug(Orange);
	}
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	grid->Render();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	FrustumCulling();
	App->editor->Render();

	return UPDATE_CONTINUE;
}

void ModuleRenderer3D::ImGuiDrawer()
{
	ImGui::Text("Background Color");
	ImGui::ColorEdit4("##Background", &BackgroundColor.x);

	if (ImGui::Checkbox("Gl_Deepth_Test", &App->renderer3D->DeepTest))
	{
		App->renderer3D->Gl_State(App->renderer3D->DeepTest, GL_DEPTH_TEST);
	}
	if (ImGui::Checkbox("Gl_Cull_Face", &App->renderer3D->CullFace))
	{
		App->renderer3D->Gl_State(App->renderer3D->CullFace, GL_CULL_FACE);
	}
	if (ImGui::Checkbox("Gl_Lighting", &App->renderer3D->GlLighting))
	{
		App->renderer3D->Gl_State(App->renderer3D->GlLighting, GL_LIGHTING);
	}
	if (ImGui::Checkbox("Gl_Color_Material", &App->renderer3D->ColorMaterial))
	{
		App->renderer3D->Gl_State(App->renderer3D->ColorMaterial, GL_COLOR_MATERIAL);
	}
	if (ImGui::Checkbox("Draw Debug", &drawdebug))
	{
	}
}
// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");
	App->Console.AddLog("Destroying 3D Renderer");
	RELEASE(grid);
	ImGui_ImplSdlGL3_Shutdown();
	SDL_GL_DeleteContext(context);

	return true;
}


void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);
	App->window->SetWindowWidhtHeight(width, height);
	App->camera->SetAspectRatio();
	App->imgui->PlayButton.x = width / 2 - 115;
	App->imgui->PlayButton.y = 20;
	App->imgui->AssetsWindow.x = 0;
	App->imgui->AssetsWindow.y = height - 260;
}

void ModuleRenderer3D::Gl_State(bool state, GLenum dummy) {


	if (state) {

		glEnable(dummy);
	}

	else {

		glDisable(dummy);
	}
}

void ModuleRenderer3D::SaveConfig(JSON_Object * root)
{
	json_object_set_number(root, "Red", ColorOverMaterial.x);
	json_object_set_number(root, "Green", ColorOverMaterial.y);
	json_object_set_number(root, "Blue", ColorOverMaterial.z);

	json_object_set_number(root, "BackGroundRed", BackgroundColor.x);
	json_object_set_number(root, "BackGroundGreen", BackgroundColor.y);
	json_object_set_number(root, "BackGroundBlue", BackgroundColor.z);

	json_object_set_boolean(root, "GL_DEPTH_TEST", DeepTest);
	json_object_set_boolean(root, "GL_CULL_FACE", CullFace);
	json_object_set_boolean(root, "GL_LIGHTNING", GlLighting);
	json_object_set_boolean(root, "GL_COLOuR_MATERIAL", ColorMaterial);
	json_object_set_boolean(root, "GL_TEXTURE_2D", Texture2D);
	json_object_set_boolean(root, "WIREFRAME", WireFrame);
}

void ModuleRenderer3D::LoadConfig(JSON_Object* root)
{
	if (json_object_get_value(root, "Red") == NULL) {
		json_object_set_value(root, "Red", json_value_init_object());
		ColorOverMaterial.x = 1;
		json_object_set_number(root, "Red", ColorOverMaterial.x);

	}
	else
	{
		ColorOverMaterial.x = json_object_get_number(root, "Red");
	}
	if (json_object_get_value(root, "Green") == NULL) {
		json_object_set_value(root, "Green", json_value_init_object());
		ColorOverMaterial.y = 1;
		json_object_set_number(root, "Green", ColorOverMaterial.y);

	}
	else
	{
		ColorOverMaterial.y = json_object_get_number(root, "Blue");
	}
	if (json_object_get_value(root, "Blue") == NULL) {
		json_object_set_value(root, "Blue", json_value_init_object());
		ColorOverMaterial.z = 1;
		json_object_set_number(root, "Blue", ColorOverMaterial.z);

	}
	else
	{
		ColorOverMaterial.z = json_object_get_number(root, "Blue");
	}
	if (json_object_get_value(root, "BackGroundRed") == NULL) {
		json_object_set_value(root, "BackGroundRed", json_value_init_object());
		BackgroundColor.x = 1;
		json_object_set_number(root, "BackGroundRed", BackgroundColor.x);

	}
	else
	{
		BackgroundColor.x = json_object_get_number(root, "BackGroundGreen");
	}
	if (json_object_get_value(root, "BackGroundGreen") == NULL) {
		json_object_set_value(root, "BackGroundGreen", json_value_init_object());
		BackgroundColor.y = 1;
		json_object_set_number(root, "BackGroundGreen", BackgroundColor.y);

	}
	else
	{
		BackgroundColor.y = json_object_get_number(root, "BackGroundGreen");
	}
	if (json_object_get_value(root, "BackGroundBlue") == NULL) {
		json_object_set_value(root, "BackGroundBlue", json_value_init_object());
		BackgroundColor.z = 1;
		json_object_set_number(root, "BackGroundBlue", BackgroundColor.z);

	}
	else
	{
		BackgroundColor.z = json_object_get_number(root, "BackGroundBlue");
	}

	if (json_object_get_boolean(root, "GL_DEPTH_TEST") == NULL) {
		json_object_set_boolean(root, "GL_DEPTH_TEST", true);
		DeepTest = true;
		json_object_set_number(root, "GL_DEPTH_TEST", DeepTest);

	}
	else
	{
		CullFace = json_object_get_number(root, "GL_DEPTH_TEST");
	}
	if (json_object_get_boolean(root, "GL_CULL_FACE") == NULL) {
		json_object_set_boolean(root, "GL_CULL_FACE", true);
		CullFace = true;
		json_object_set_number(root, "GL_CULL_FACE", CullFace);

	}
	else
	{
		CullFace = json_object_get_number(root, "GL_CULL_FACE");
	}

	if (json_object_get_boolean(root, "GL_LIGHTNING") == NULL) {
		json_object_set_boolean(root, "GL_LIGHTNING", true);
		GlLighting = true;
		json_object_set_number(root, "GL_LIGHTNING", GlLighting);

	}
	else
	{
		GlLighting = json_object_get_number(root, "GL_LIGHTNING");
	}
	if (json_object_get_boolean(root, "GL_COLOuR_MATERIAL") == NULL) {
		json_object_set_boolean(root, "GL_COLOuR_MATERIAL", true);
		ColorMaterial = true;
		json_object_set_number(root, "GL_COLOuR_MATERIAL", ColorMaterial);

	}
	else
	{
		ColorMaterial = json_object_get_number(root, "GL_COLOuR_MATERIAL");
	}
	if (json_object_get_boolean(root, "GL_TEXTURE_2D") == NULL) {
		json_object_set_boolean(root, "GL_TEXTURE_2D", true);
		Texture2D = true;
		json_object_set_number(root, "GL_TEXTURE_2D", Texture2D);

	}
	else
	{
		Texture2D = json_object_get_number(root, "GL_TEXTURE_2D");
	}
	if (json_object_get_boolean(root, "WIREFRAME") == NULL) {
		json_object_set_boolean(root, "WIREFRAME", true);
		WireFrame = false;
		json_object_set_number(root, "WIREFRAME", WireFrame);

	}
	else
	{
		WireFrame = json_object_get_number(root, "WIREFRAME");
	}

}

void ModuleRenderer3D::FrustumCulling()
{
	CompCamera* camera = (CompCamera*)App->editor->GetRoot()->FindComponent(Component_Camera);

	std::vector<GameObject*> cullingVector;
	if (camera->GetFrustumCulling() == false)
	{
		App->editor->GetQuadtree()->root->CollectIntersectionsFrustum(cullingVector, App->camera->FrustumPick);
		App->editor->CollectIntersectionsFrustumDynamicObjects(cullingVector, App->camera->FrustumPick);
	}
	else 
	{
		App->editor->GetQuadtree()->root->CollectIntersectionsFrustum(cullingVector, camera->frustum);
		App->editor->CollectIntersectionsFrustumDynamicObjects(cullingVector, camera->frustum);
	}

	for (std::vector<GameObject*>::const_iterator it = cullingVector.begin(); it != cullingVector.end(); ++it)
	{
		for (int i = 0; i < (*it)->components.size(); ++i)
		{
			if ((*it)->components[i]->GetType() == Component_Mesh)
			{
				CompTransform* tmpTrans = (CompTransform*)(*it)->FindComponent(Component_Transform);
				CompMesh* toDraw = (CompMesh*)(*it)->FindComponent(Component_Mesh);
				if (tmpTrans != nullptr && toDraw != nullptr) 
				{
					glPushMatrix();
					if (tmpTrans != nullptr)
					{
						glMultMatrixf((GLfloat*)tmpTrans->GetTransMatrix().Transposed().ptr());
					}
						Render((*it));
						glPopMatrix();
				}
			}
		}
	}
	glPopMatrix();
	glUseProgram(0);
}

void ModuleRenderer3D::Render(GameObject * toDraw)
{
	CompMesh* CMesh = (CompMesh*)toDraw->FindComponent(Component_Mesh);
		if (CMesh->resourceMesh != nullptr)
		{
			if (CMesh->resourceMesh->numVertices > 4)
			{
				if (CMesh->drawdebug)
				{
					CMesh->DrawDebug();
				}
				if (WireFrame == true)
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				else
				{
					glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				}
			
					if (CMesh->resourceMesh->idNormals > 0)
					{
						glEnable(GL_LIGHTING);
						glEnableClientState(GL_NORMAL_ARRAY);
						if (CMesh->deformableMesh == nullptr)
						{
							glBindBuffer(GL_ARRAY_BUFFER, CMesh->resourceMesh->idNormals);
						}
						else
						{
							glBindBuffer(GL_ARRAY_BUFFER, CMesh->deformableMesh->idNormals);
						}
						glNormalPointer(GL_FLOAT, 0, NULL);
					}
			

				if (CMesh->resourceMesh->idTexCoords > 0)
				{
					CompMaterial* mat = (CompMaterial*)(toDraw->FindComponent(Component_Material));
					if (mat != nullptr)
					{
						glEnable(GL_TEXTURE_2D);
						glBindTexture(GL_TEXTURE_2D, mat->GetTextureID());
					}
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glBindBuffer(GL_ARRAY_BUFFER, CMesh->resourceMesh->idTexCoords);
					glTexCoordPointer(3, GL_FLOAT, 0, NULL);
				}
				if (CMesh->resourceMesh->idVertices != 0) {
				glEnableClientState(GL_VERTEX_ARRAY);
				if (CMesh->deformableMesh == nullptr)
				{
					glBindBuffer(GL_ARRAY_BUFFER, CMesh->resourceMesh->idVertices);
				}
				else
				{
					glBindBuffer(GL_ARRAY_BUFFER, CMesh->deformableMesh->idVertices);
				}
				glVertexPointer(3, GL_FLOAT, 0, NULL);
			}
			if (CMesh->resourceMesh->idIndices != 0)				{
				glEnableClientState(GL_ELEMENT_ARRAY_BUFFER);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, CMesh->resourceMesh->idIndices);
				glDrawElements(GL_TRIANGLES, CMesh->resourceMesh->numIndices, GL_UNSIGNED_INT, NULL);
			}
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

