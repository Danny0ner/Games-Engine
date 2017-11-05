#include "Application.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"
#include "ModuleRenderer3D.h"
#include "mmgr\mmgr.h"
#include "CompCamera.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "GameObject.h"
#include "ImGui\ImGuizmo.h"
ModuleEditor::ModuleEditor(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	name = "Editor";
}

ModuleEditor::~ModuleEditor()
{
	
}

bool ModuleEditor::Init()
{
	return true;
}

bool ModuleEditor::Start()
{
	root = new GameObject();
	root->SetName("Root");
	CompCamera* cam = new CompCamera();
	cam->frustumCulling = false;
	root->AddComponent(cam);
	Quadroot = new Octree(AABB(float3(-100, -20, -100), float3(100, 120, 100)));

	return true;
}

update_status ModuleEditor::Update(float dt)
{
	if (App->input->GetKey(SDL_SCANCODE_H) == KEY_DOWN) 
		SerializeScene("Scene test");
	return UPDATE_CONTINUE;
}

bool ModuleEditor::CleanUp()
{
	delete Quadroot;
	delete root;
	return true;
}

void ModuleEditor::Render()
{
	root->Update();
	Quadroot->DrawDebug(Orange);

	if (selected != nullptr)
	{
		selected->ShowInspector();
		//DrawGuizmo(selected);
	}

}

void ModuleEditor::ShowEditor()
{
	root->OnEditor();
}

GameObject * ModuleEditor::GetRoot()
{
	return root;
}

Octree * ModuleEditor::GetQuadtree()
{
	return Quadroot;
}

GameObject * ModuleEditor::CreateNewGameObject(const char * path)
{
	GameObject* ret = App->geometryloader->LoadGameObject(path);
	if (ret != nullptr)
	{
		//root->DeleteChilds();
		delete Quadroot;
		root->AddChild(ret);
		AABB Enclosing_Box;
		Enclosing_Box.SetNegativeInfinity();
		for (std::vector<GameObject*>::const_iterator tmp = Static_Vector.begin(); tmp != Static_Vector.end(); tmp++)
		{
			CompMesh* tmpmesh = (CompMesh*)(*tmp)->FindComponent(Component_Mesh);
			CompTransform* tmptransf = (CompTransform*)(*tmp)->FindComponent(Component_Transform);
			AABB TempBox = tmpmesh->enclosingBox;
			TempBox.TransformAsAABB(tmptransf->GetTransMatrix());
			Enclosing_Box.Enclose(TempBox);

		}

		Quadroot = new Octree(Enclosing_Box);
		for (std::vector<GameObject*>::const_iterator tmp = Static_Vector.begin(); tmp != Static_Vector.end(); tmp++)
		{

			Quadroot->Insert((*tmp));


		}
		//App->camera->CenterToGO(ret);
	}
	else
	{
		LOG("Couldn't load .fbx file!");
	}

	return ret;
}

void ModuleEditor::SelectGameObject(GameObject * Selected)
{
	if (Selected == NULL)
	{
		if (this->selected != nullptr) {
			this->selected->selected = false;
			this->selected = nullptr;
			return;
		}
	}
	if (this->selected != Selected)
	{
		if (this->selected != nullptr)
		{
			this->selected->selected = false;
		}
		Selected->selected = true;
		this->selected = Selected;
		
	}
}


GameObject* ModuleEditor::CastRay(const LineSegment& Segment, float Distance)
{
	Distance = 100000;
	GameObject* Select = nullptr;
	TestRay(Segment, &Distance, Select);
	return Select;

}

void ModuleEditor::TestRay(const LineSegment& Segment, float* Distance, GameObject* &Select)
{
	//std::vector<GameObject*> Objects;
	//Quadroot->root->CollectIntersectionsLine(Objects, Segment);

	for (std::vector<GameObject*>::const_iterator tmp = Static_Vector.begin(); tmp != Static_Vector.end(); tmp++)
	{
		// Look for meshes, nothing else can be "picked" from the screen
	
		CompMesh* Mesh = (CompMesh*)(*tmp)->FindComponent(Component_Mesh);
		

		if (Mesh != nullptr)
		{
			CompTransform* transf = (CompTransform*)(*tmp)->FindComponent(Component_Transform);

			// test all triangles
			LineSegment SegmentLocal(Segment);
			//SegmentLocal.Transform(transf->GetTransMatrix().Inverted());
			Triangle tri;
			AABB box = Mesh->enclosingBox;
			box.TransformAsAABB(transf->GetTransMatrix());
			float LocalDistance = 0;
			float LocalHitPoint;
			if(SegmentLocal.Intersects(box, LocalDistance, LocalHitPoint))
			{
				if (&LocalDistance < Distance)
				{
					Distance = &LocalDistance;
					Select = (*tmp);
				}
			}
			/*for (uint i = 0; i <= Mesh->numIndices +1;)
			{
				tri.a.Set(Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]]);
				tri.b.Set(Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]]);
				tri.c.Set(Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]], Mesh->vertices[Mesh->indices[i++]]);

				float* LocalDistance=0;
				float3 LocalHitPoint;
				LocalHitPoint.x = 0; LocalHitPoint.y = 0; LocalHitPoint.z = 0;
				if (SegmentLocal.Intersects(tri, LocalDistance, &LocalHitPoint))
				{
					if (LocalDistance < Distance)
					{
						Distance = LocalDistance;
						Select = (*tmp);
					}
				}
			}*/
		}
	}



}

void ModuleEditor::SerializeScene(const char * filename)
{
	Configuration save;
	save.AddArray("Scene Game Objects");

	root->OnSerialize(save);

	char* buffer = nullptr;
	uint fileSize = save.SaveFile(&buffer, "Scene save");
	App->filesystem->SaveFile(filename, buffer, fileSize, FileType::fileScene);
	RELEASE_ARRAY(buffer);
}

void ModuleEditor::LoadScene(const char * fileTitle)
{
	Configuration load(fileTitle);

	if (load.IsValueValid() == true)
	{
		root->DeleteChilds();
		selected = nullptr;
		for (int i = 0; i < load.GetArraySize("Scene Game Objects"); i++)
		{
			GameObject* tmp = new GameObject();
			Configuration testC = load.GetArray("Scene Game Objects", i);
			tmp->Deserialize(testC);
		}
		//App->Console.AddLog("Load completed in %i ms", saveLoadTimer.Read());
		//saveLoadTimer.Stop();
	}
	else
	{
		LOG("Scene file not valid.");
	}
}
/*
void ModuleEditor:EditTransform(GameObject* Select)
{
	static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
	static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
	if (ImGui::IsKeyPressed(90))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	if (ImGui::IsKeyPressed(69))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	if (ImGui::IsKeyPressed(82)) // r Key
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
		mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
		mCurrentGizmoOperation = ImGuizmo::ROTATE;
	ImGui::SameLine();
	if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
		mCurrentGizmoOperation = ImGuizmo::SCALE;
	float matrixTranslation[3], matrixRotation[3], matrixScale[3];
	ImGuizmo::DecomposeMatrixToComponents(matrix.m16, matrixTranslation, matrixRotation, matrixScale);
	ImGui::InputFloat3("Tr", matrixTranslation, 3);
	ImGui::InputFloat3("Rt", matrixRotation, 3);
	ImGui::InputFloat3("Sc", matrixScale, 3);
	ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m16);

	if (mCurrentGizmoOperation != ImGuizmo::SCALE)
	{
		if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
			mCurrentGizmoMode = ImGuizmo::LOCAL;
		ImGui::SameLine();
		if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
			mCurrentGizmoMode = ImGuizmo::WORLD;
	}
	static bool useSnap(false);
	if (ImGui::IsKeyPressed(83))
		useSnap = !useSnap;
	ImGui::Checkbox("", &useSnap);
	ImGui::SameLine();
	vec_t snap;
	switch (mCurrentGizmoOperation)
	{
	case ImGuizmo::TRANSLATE:
		snap = config.mSnapTranslation;
		ImGui::InputFloat3("Snap", &snap.x);
		break;
	case ImGuizmo::ROTATE:
		snap = config.mSnapRotation;
		ImGui::InputFloat("Angle Snap", &snap.x);
		break;
	case ImGuizmo::SCALE:
		snap = config.mSnapScale;
		ImGui::InputFloat("Scale Snap", &snap.x);
		break;
	}
	ImGuiIO& io = ImGui::GetIO();
	ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
	ImGuizmo::Manipulate(camera.mView.m16, camera.mProjection.m16, mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m16, NULL, useSnap ? &snap.x : NULL);
}
*/