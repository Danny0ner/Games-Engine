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
	CompBone(MeshBone* bone);

	~CompBone();

	void Update(float dt);


	void OnEditor()override;

	void OnSave(Configuration& data) const;
	void OnLoad(Configuration& data);

	void DebugDraw() const;

	void SetMeshToDeform(CompMesh* mesh);

public:
	MeshBone* actualbone = nullptr;
	CompMesh* MeshToDeform = nullptr;
	bool drawdebug = false;

};
#endif
