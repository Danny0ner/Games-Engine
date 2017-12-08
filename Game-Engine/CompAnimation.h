#ifndef COMPANIMATION_
#define COMPANIMATION_

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceAnimation.h"

class CompAnimation : public Component
{
public:
	CompAnimation();

	~CompAnimation();

	void Update(float dt);

	void DrawDebug() const;
	void OnEditor()override;

	void OnSave(Configuration& data) const;
	void OnLoad(Configuration& data);

	void AddResourceByName(std::string filename);
	void AddResource(int uid);
	ResourceAnimation * GetResourceAnim();
public:
	float animetime = 0.0f;
	bool drawdebug = false;
	bool ChangingAnimation = false;
	bool Interpolation = true;
	ResourceAnimation* resourceAnim = nullptr;
};
#endif