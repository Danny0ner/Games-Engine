#include <experimental\filesystem>
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
	//std::string yeahyo;
	//for (std::experimental::filesystem::directory_entry p : std::experimental::filesystem::recursive_directory_iterator("Assets"))
		//yeahyo = p.path().string();
	//std::experimental::filesystem::create_directory("Assets/YEHAOTY");
	return true;
}

update_status ModuleEditor::Update(float dt)
{
	if (StaticVecSize != Static_Vector.size())
		ReCreateQuadtree();
	StaticVecSize = Static_Vector.size();
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
		((CompTransform*)ret->FindComponent(Component_Transform))->UpdateChildsTransMatrixNow();
		ReCreateQuadtree();
		StaticVecSize = Static_Vector.size();
		//App->camera->CenterToGO(ret);
	}
	else
	{
		LOG("Couldn't load .fbx file!");
	}

	return ret;
}

void ModuleEditor::ReCreateQuadtree()
{
	AABB Enclosing_Box;
	Enclosing_Box.SetNegativeInfinity();
	for (std::vector<GameObject*>::const_iterator tmp = Static_Vector.begin(); tmp != Static_Vector.end(); tmp++)
	{
		CompMesh* tmpmesh = (CompMesh*)(*tmp)->FindComponent(Component_Mesh);
		CompTransform* tmptransf = (CompTransform*)(*tmp)->FindComponent(Component_Transform);
		if (tmpmesh != nullptr)
		{
			AABB TempBox = tmpmesh->enclosingBox;
			TempBox.TransformAsAABB(tmptransf->GetTransMatrix());
			Enclosing_Box.Enclose(TempBox);
		}
	}

	Quadroot = new Octree(Enclosing_Box);
	for (std::vector<GameObject*>::const_iterator tmp = Static_Vector.begin(); tmp != Static_Vector.end(); tmp++)
	{

		Quadroot->Insert((*tmp));


	}
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
	TestRay(Segment, Distance, Select);
	return Select;

}

void ModuleEditor::TestRay(const LineSegment& Segment, float& Distance, GameObject* &Select)
{
	std::vector<GameObject*> Objects;
	Quadroot->root->CollectIntersectionsLine(Objects, Segment);
	CollectIntersectionsLineDynamicObjects(Objects, Segment);

	for (std::vector<GameObject*>::const_iterator tmp = Objects.begin(); tmp != Objects.end(); tmp++)
	{
		// Look for meshes, nothing else can be "picked" from the screen
	
		CompMesh* Mesh = (CompMesh*)(*tmp)->FindComponent(Component_Mesh);
		

		if (Mesh != nullptr)
		{
			CompTransform* transf = (CompTransform*)(*tmp)->FindComponent(Component_Transform);

			// test all triangles
			LineSegment SegmentLocal(Segment);
			SegmentLocal.Transform(transf->GetTransMatrix().Inverted());
			AABB box = Mesh->enclosingBox;
			float LocalDistance = 0;
			float LocalHitPoint;
			if (Mesh->resourceMesh->numIndices > 9 || Mesh->resourceMesh->numVertices % 3 == 0)
			{
				for (int i = 0; i < Mesh->resourceMesh->numIndices; i += 3)
				{
					Triangle face(float3(Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i] * 3], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i] * 3 + 1], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i] * 3 + 2]),
						float3(Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 1] * 3], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 1] * 3 + 1], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 1] * 3 + 2]),
						float3(Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 2] * 3], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 2] * 3 + 1], Mesh->resourceMesh->vertices[Mesh->resourceMesh->indices[i + 2] * 3 + 2]));

					float LocalDistance = 0;
					float3 LocalHitPoint;
					LocalHitPoint.x = 0; LocalHitPoint.y = 0; LocalHitPoint.z = 0;
					if (SegmentLocal.Intersects(face, &LocalDistance, &LocalHitPoint))
					{
						if (LocalDistance < Distance)
						{
							Distance = LocalDistance;
							Select = (*tmp);
						}
					}
				}
			}
		}
	}



}

void ModuleEditor::CollectIntersectionsLineDynamicObjects(std::vector<GameObject*> &objects, const LineSegment& line) const
{
	for (std::vector<GameObject*>::const_iterator it = Dynamic_Vector.begin(); it != Dynamic_Vector.end(); ++it)
	{
		CompMesh* tmp = (CompMesh*)(*it)->FindComponent(Component_Mesh);
		CompTransform* transf = (CompTransform*)(*it)->FindComponent(Component_Transform);
		AABB Enclosing_Box = tmp->enclosingBox;
		Enclosing_Box.TransformAsAABB(transf->GetTransMatrix());
		if (tmp != nullptr)
		{
			if (line.Intersects(Enclosing_Box))
			{
				objects.push_back(*it);
			}
		}
	}
}

void ModuleEditor::SerializeScene(const char * filename)
{
	Configuration save;

	save.AddArray("Scene Resources");
	App->resources->SaveResources(save);
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
		App->resources->LoadResources(load);
		for (int i = 0; i < load.GetArraySize("Scene Game Objects"); i++)
		{
			GameObject* tmp = new GameObject();
			Configuration testC = load.GetArray("Scene Game Objects", i);
			tmp->Deserialize(testC);
		}

	}
	else
	{
		LOG("Scene file not valid.");
	}
	ReCreateQuadtree();
	if(root->childs.size()>0)((CompTransform*)root->childs[0]->FindComponent(Component_Transform))->UpdateChildsTransMatrixNow();
}

void ModuleEditor::LockImput()
{
	LockedImput = true;
}
void ModuleEditor::UnlockImput() 
{
	LockedImput = false;
}
bool ModuleEditor:: IsImputLocked()
{
	return LockedImput;
}


