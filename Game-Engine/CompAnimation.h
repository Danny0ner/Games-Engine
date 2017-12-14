#ifndef COMPANIMATION_
#define COMPANIMATION_

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceAnimation.h"

enum AnimationState
{
	A_PLAY,
	A_STOP,
	A_BLENDING
};
struct AnimationClip
{
	std::string name = "Animation Clip";
	bool Loop = true;
	float StartFrameTime = 0.0f;
	float EndFrameTime = 0.0f;
	bool ActuallyRunning = false;
};

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

	void SetBlendingBone(GameObject* Bone, PositionKey* ActualPos, PositionKey* NextPos, RotationKey* ActualRot, RotationKey* NextRot, float time);

	void SetActualRotationKey(RotationKey* &Actual, RotationKey* &Next,Bone* bone, int p, float time);
	void SetActualPositionKey(PositionKey* &Actual, PositionKey* &Next, Bone* bone, int p, float time);

	void SetBonePosition(GameObject* Bone, PositionKey* ActualPos, PositionKey* NextPos);
	void SetBoneRotation(GameObject* Bone, RotationKey* ActualRot, RotationKey* NextRot);

	float3 GetBonePosition(GameObject* Bone, PositionKey* ActualPos, PositionKey* NextPos);
	Quat GetBoneRotation(GameObject* Bone, RotationKey* ActualRot, RotationKey* NextRot);

	ResourceAnimation * GetResourceAnim();
public:
	float blendingtime = 0.5f;
	float blendtime = 0.0f;
	float animetime = 0.0f;
	bool drawdebug = false;
	bool ChangingAnimation = false;
	bool Interpolation = true;
	float TicksPerSecond = 0;
	ResourceAnimation* resourceAnim = nullptr;
	std::vector<AnimationClip*> animationclips;
	AnimationClip* ActualClip = nullptr;
	AnimationClip* LastClip = nullptr;
	AnimationState AnimState=A_PLAY;
};
#endif