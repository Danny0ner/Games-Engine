#ifndef COMPANIMATION_
#define COMPANIMATION_

#include "Globals.h"
#include "Component.h"
#include "MathGeo\Geometry\AABB.h"
#include "ResourceAnimation.h"
#include <map>

enum AnimationState
{
	A_PLAY,
	A_STOP,
	A_PAUSED
};
struct AnimationClip
{
	std::string name = "Animation Clip";
	bool loop = true;
	float time = start_frame_time;
	float start_frame_time = 0.0f;
	float end_frame_time = 0.0f;
	bool ActuallyRunning = false;
	bool finished = true;
	AnimationState state = A_PLAY;

	void RestartAnimationClip();
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

	float3 GetBonePosition(GameObject* Bone, PositionKey* ActualPos, PositionKey* NextPos, float time);
	Quat GetBoneRotation(GameObject* Bone, RotationKey* ActualRot, RotationKey* NextRot, float time);


	ResourceAnimation * GetResourceAnim();

	void AnimationMoves(); //Only for the demo
public:
	float blendingtime = 0.5f;

	float blendtime = 0.0f;
	float animetime = 0.0f;

	float nextanimetime = 0.0f;
	bool drawdebug = false;
	bool ChangingAnimation = false;
	bool Interpolation = true;
	float TicksPerSecond = 0;
	ResourceAnimation* resource = nullptr;
	std::vector<AnimationClip*> animation_clips;
	AnimationClip* ActualClip = nullptr;
	AnimationClip* LastClip = nullptr;
	AnimationState AnimState=A_PLAY;

	bool bonesplaceds = false;

	//std::map<const char*, GameObject*> bonesGOs;
	std::vector<std::pair<GameObject*, const Bone*>> bone_update_vector;
};
#endif