#include "Application.h"
#include "ModuleEditor.h"
#include "GeometryLoader.h"
#include "ModuleRenderer3D.h"
#include "mmgr\mmgr.h"
#include "CompCamera.h"
#include "CompMesh.h"
#include "CompTransform.h"
#include "GameObject.h"
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
	root->AddComponent(cam);
	Quadroot = new Octree(AABB(float3(-100, -20, -100), float3(100, 120, 100)));

	return true;
}

update_status ModuleEditor::Update(float dt)
{
	
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
	if (this->selected != Selected)
	{
		if (this->selected != nullptr)
		{
			this->selected->selected = false;
		}
		Selected->selected = true;
		this->selected = Selected;
	}
	else
	{
		this->selected->selected = false;
		this->selected = nullptr;
	}
}


GameObject* ModuleEditor::CastRay(const LineSegment& Segment, float Distance)
{
	Distance = inf;
	GameObject* Select = nullptr;
	TestRay(Segment, &Distance, Select);
	return Select;

}

void ModuleEditor::TestRay(const LineSegment& Segment, float* Distance, GameObject* &Select)
{
	std::vector<GameObject*> Objects;
	Quadroot->root->CollectIntersectionsLine(Objects, Segment);

	for (std::vector<GameObject*>::const_iterator tmp = Objects.begin(); tmp != Objects.end(); tmp++)
	{
		// Look for meshes, nothing else can be "picked" from the screen
		std::vector<CompMesh*> Meshes;
		Meshes.push_back((CompMesh*)(*tmp)->FindComponent(Component_Mesh));
		

		if (Meshes.size() > 0)
		{
			const CompMesh* Mesh = (const CompMesh*)Meshes[0];
			CompTransform* transf = (CompTransform*)(*tmp)->FindComponent(Component_Transform);

			// test all triangles
			LineSegment SegmentLocal(Segment);
			SegmentLocal.Transform(transf->GetTransMatrix().Inverted());

			Triangle tri;
			for (uint i = 0; i < Mesh->numVertices;)
			{

				tri.a.Set(Mesh->numVertices[&Mesh->indices[i++]], Mesh->numVertices[&Mesh->indices[i]], Mesh->numVertices[&Mesh->indices[i]]);
				tri.b.Set(Mesh->numVertices[&Mesh->indices[i++]], Mesh->numVertices[&Mesh->indices[i]], Mesh->numVertices[&Mesh->indices[i]]);
				tri.c.Set(Mesh->numVertices[&Mesh->indices[i++]], Mesh->numVertices[&Mesh->indices[i]], Mesh->numVertices[&Mesh->indices[i]]);

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
			}
		}
	}



}
