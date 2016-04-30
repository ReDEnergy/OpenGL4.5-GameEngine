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

void AnimationController::SetAnimationTime(float animationTime)
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
	return animationTime;
}

const std::unordered_map<std::string, SkeletalJoint*>& AnimationController::GetJointsList() const
{
	return skeletalJoints;
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
	animationTime = fmod(TimeInTicks, (float)currentAnimation->mDuration);

	//rootJoint->globalTransform = rootTransform;
	UpdateJointTransform(rootJoint, animationTime);
	//rootJoint->UpdateSkeleton(rootTransform);
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
	joint->transform->SetLocalPosition(position / 100.0f);
	joint->transform->SetLocalRotation(rotation);
	joint->transform->SetScale(glm::vec3(0.01f));
	joint->finalTransformation = joint->transform->GetModel() * joint->boneOffset;

	for (auto J : joint->GetChildren()) {
		UpdateJointTransform((SkeletalJoint*)J, animationTime);
	}
}

void AnimationController::SetJointToBindPose(SkeletalJoint* joint)
{
	// Update transform
	joint->transform->SetLocalRotation(glm::quat());
	joint->finalTransformation = joint->transform->GetModel() * joint->boneOffset;

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