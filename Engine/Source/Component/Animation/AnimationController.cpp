#include "AnimationController.h"

#include <include/utils.h>
#include <include/gl_utils.h>
#include <include/glm_utils.h>
#include <include/assimp_utils.h>

#include <Component/Transform/Transform.h>
#include <Component/SkinnedMesh.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/TextureManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

#include <SkeletalSystem/SkeletalJoint.h>

#include <Utils/GPU.h>

inline uint GetNextAnimationKey(uint currentKey, uint maxKeys)
{
	return (currentKey + 1) % maxKeys;
}

AnimationController::AnimationController()
{
	playbackState = true;
	controlState = true;
	currentAnimation = nullptr;
	skinnedMesh = nullptr;
	rootJoint = nullptr;
	animationTime = 0;
}


AnimationController::~AnimationController() {
}

void AnimationController::Setup(SkinnedMesh * mesh)
{
	skinnedMesh = mesh;

	skeletalJoints = mesh->GetSkeletonCopy(rootJoint);
	nrBones = mesh->GetNumberOfBones();
	boneTransform.resize(nrBones);
	rootTransform = mesh->GetRootTransform();
	globalInvTransform = mesh->GetGlobalInverse();

	Manager::Scene->AddObject((GameObject*)rootJoint);

	rootJoint->UpdateSkeleton();
	UpdateGPUData();

}

void AnimationController::InitControlSkeleton()
{
}

void AnimationController::Update()
{
	if (currentAnimation == nullptr)
		return;
	if (rootJoint)
	{
		if (playbackState == false)
		{
			rootJoint->UpdateSkeleton();
			UpdateGPUData();
			return;
		}
		UpdateAnimation();
	}
	UpdateGPUData();
}

void AnimationController::SetDefaultPose()
{
	playbackState = false;
	SetJointToBindPose(rootJoint);
	UpdateGPUData();
}

void AnimationController::SetAnimation(unsigned int animationID)
{
	auto animation = skinnedMesh->GetAnimationByID(animationID);
	if (animation) {
		currentAnimation = animation;
		UpdateAnimationNodesMapping();
	}
}

void AnimationController::SetAnimation(const char * animationName)
{
	auto animation = skinnedMesh->GetAnimation(animationName);
	if (animation) {
		currentAnimation = animation;
		UpdateAnimationNodesMapping();
	}
}

void AnimationController::SetAnimationTime(double animationTime)
{
	//rootJoint->globalTransform = rootTransform;
	UpdateJointTransform(rootJoint, animationTime);
}

void AnimationController::SetPlayback(bool value)
{
	playbackState = value;
}

bool AnimationController::TogglePlayback()
{
	playbackState = !playbackState;
	return playbackState;
}

float AnimationController::GetAnimationTime() const
{
	return static_cast<float>(animationTime);
}

const std::unordered_map<std::string, SkeletalJoint*>& AnimationController::GetJointsList() const
{
	return skeletalJoints;
}

void AnimationController::UpdateGPUData()
{
	for (auto &bone : skeletalJoints) {
		auto id = bone.second->GetJointID();
		boneTransform[id] = bone.second->GetBoneTransform();
	}
}

void AnimationController::UpdateAnimation()
{
	auto timeInSeconds = Engine::GetElapsedTime();
	auto TimeInTicks = timeInSeconds * currentAnimation->mTicksPerSecond;
	animationTime = fmod(TimeInTicks, currentAnimation->mDuration);

	//rootJoint->globalTransform = rootTransform;
	UpdateJointTransform(rootJoint, static_cast<float>(animationTime));
	//rootJoint->UpdateSkeleton(rootTransform);
}

void AnimationController::UpdateAnimationNodesMapping()
{
	for (auto &bone : skeletalJoints) {
		bone.second->FindAnimationNode(currentAnimation);
	}
}

void AnimationController::UpdateJointTransform(SkeletalJoint* joint, double animationTime)
{
	// Interpolate transformation between 2 keyframes
	glm::vec3 position = assimp::CalcInterpolatedPosition(joint->GetAssimpAnimationNode(), animationTime, currentAnimation->mDuration);
	glm::quat rotation = assimp::CalcInterpolatedRotation(joint->GetAssimpAnimationNode(), animationTime, currentAnimation->mDuration);
	//glm::vec3 scale = assimp::CalcInterpolatedScaling(joint->pAnimationNode, animationTime, currentAnimation->mDuration);

	// Update transform
	joint->transform->SetLocalPosition(position / 100.0f);
	joint->transform->SetRelativeRotation(rotation);
	joint->transform->SetScale(glm::vec3(0.01f));
	joint->UpdateBoneTransform();

	for (auto J : joint->GetChildren()) {
		UpdateJointTransform((SkeletalJoint*)J, animationTime);
	}
}

void AnimationController::SetJointToBindPose(SkeletalJoint* joint)
{
	// Update transform
	joint->transform->SetRelativeRotation(glm::quat());
	joint->UpdateBoneTransform();

	for (auto J : joint->GetChildren()) {
		SetJointToBindPose((SkeletalJoint*)J);
	}
}

void AnimationController::BindSkeletonInfo(const Shader *shader) const
{
	glUniform1f(shader->loc_animated, currentAnimation ? true : false);
	if (nrBones)
		glUniformMatrix4fv(shader->loc_bones, nrBones, GL_FALSE, glm::value_ptr(boneTransform[0]));
	CheckOpenGLError();
}
