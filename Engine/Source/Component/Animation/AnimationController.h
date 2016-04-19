#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/dll_export.h>
#include <include/glm.h>
#include <include/utils.h>

#include <unordered_map>

class Shader;
class SkeletalJoint;
class Transform;
class SkinnedMesh;

class DLLExport AnimationController
{
	public:
		AnimationController();
		~AnimationController();

		void Setup(SkinnedMesh *mesh);
		void BindSkeletonInfo(const Shader* shader) const;

		void Update();
		void SetDefaultPose();
		void SetAnimation(unsigned int animationID);
		void SetAnimation(const char *animationName);
		void SetAnimationTime(float animationTime);
		void SetPlayback(bool value);
		bool TogglePlayback();
		float GetAnimationTime() const;

	private:
		void InitControlSkeleton();

		void UpdateGPUData();
		void UpdateAnimation();
		void UpdateAnimationNodesMapping();
		void UpdateJointTransform(SkeletalJoint * joint, float animationTime);

	public:
		uint nrBones;
		SkinnedMesh *skinnedMesh;
		std::unordered_map<std::string, SkeletalJoint*> skeletalJoints;
		std::vector<glm::mat4> boneTransform;

		SkeletalJoint *rootJoint;
		glm::mat4 rootTransform;
		glm::mat4 globalInvTransform;

		#ifdef ENGINE_DLL_EXPORTS
		aiAnimation *currentAnimation;
		#endif
		bool playbackState;
		bool controlState;
		float animationTime;
};