#ifndef COMPBONE_
#define COMPBONE_

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceSkeleton.h"
#include "CompMesh.h"

class CompBone : public Component
{
public:
	CompBone(MeshBone* bone, CompMesh* meshtodeform);

	~CompBone();

	void Update(float dt);


	void OnEditor()override;

	void OnSave(Configuration& data) const;
	void OnLoad(Configuration& data);

	void DebugDraw() const;
	//void AddResourceByName(std::string filename);
	//void AddResource(int uid);

public:
	MeshBone* actualbone = nullptr;
	CompMesh* MeshToDeform = nullptr;
	bool drawdebug = false;

};
#endif
