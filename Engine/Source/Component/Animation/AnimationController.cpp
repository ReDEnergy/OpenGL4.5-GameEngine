//#include <pch.h>
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
}


AnimationController::~AnimationController() {
}

void AnimationController::Setup(SkinnedMesh * mesh)
{
	skinnedMesh = mesh;
	currentAnimation = nullptr;

	rootJoint = mesh->GetSkeletonInfo(skeletalJoints);
	nrBones = mesh->GetNumberOfBones();
	boneTransform.resize(nrBones);
	rootTransform = mesh->GetRootTransform();
	globalInvTransform = mesh->GetGlobalInverse();

	Manager::Scene->AddObject((GameObject*)rootJoint);
}

void AnimationController::InitControlSkeleton()
{
}

void AnimationController::Update()
{
	if (rootJoint) {
		if (currentAnimation == nullptr) {
			rootJoint->globalTransform = rootTransform;
			rootJoint->UpdateSkeletonTPose(globalInvTransform);
			UpdateGPUData();
			return;
		}

		if (playbackState == false) {
			rootJoint->globalTransform = rootTransform;
			rootJoint->UpdateSkeleton(globalInvTransform);
			UpdateGPUData();
			return;
		}
		UpdateAnimation();
	}
	UpdateGPUData();
}

void AnimationController::SetAnimation(const char * animationName)
{
	auto animation = skinnedMesh->GetAnimation(animationName);
	if (animation) {
		currentAnimation = animation;
		UpdateAnimationNodesMapping();
	}
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

void AnimationController::UpdateGPUData()
{
	for (auto &bone : skeletalJoints) {
		auto id = bone.second->GetJointID();
		boneTransform[id] = bone.second->finalTransformation;
	}
}

void AnimationController::UpdateAnimation()
{
	float timeInSeconds = (float)glfwGetTime();
	float TimeInTicks = timeInSeconds * (float)currentAnimation->mTicksPerSecond;
	float animationTime = fmod(TimeInTicks, (float)currentAnimation->mDuration);

	rootJoint->globalTransform = rootTransform;
	UpdateJointTransform(rootJoint, animationTime);
}

void AnimationController::UpdateAnimationNodesMapping()
{
	for (auto &bone : skeletalJoints) {
		bone.second->pAnimationNode = assimp::FindNodeAnim(currentAnimation, bone.first);
	}
}

void AnimationController::UpdateJointTransform(SkeletalJoint* joint, float animationTime)
{
	// Interpolate transformation between 2 keyframes
	glm::vec3 position = assimp::CalcInterpolatedPosition(joint->pAnimationNode, animationTime, currentAnimation->mDuration);
	glm::quat rotation = assimp::CalcInterpolatedRotation(joint->pAnimationNode, animationTime, currentAnimation->mDuration);
	glm::vec3 scale = assimp::CalcInterpolatedScaling(joint->pAnimationNode, animationTime, currentAnimation->mDuration);

	// Update transform
	joint->transform->SetWorldPosition(position);
	joint->transform->SetWorldRotationAndScale(rotation, scale);
	joint->UpdateTransform(globalInvTransform);

	for (auto J : joint->GetChildren()) {
		UpdateJointTransform((SkeletalJoint*)J, animationTime);
	}
}

void AnimationController::Render(const Shader *shader)
{
	glUniform1f(shader->loc_animated, currentAnimation ? true : false);
	if (nrBones)
		glUniformMatrix4fv(shader->loc_bones[0], nrBones, GL_FALSE, glm::value_ptr(boneTransform[0]));
	skinnedMesh->Mesh::Render(shader);
}