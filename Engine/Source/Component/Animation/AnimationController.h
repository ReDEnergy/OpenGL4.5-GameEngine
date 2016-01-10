#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/dll_export.h>
#include <include/gl.h>
#include <include/glm.h>
#include <include/utils.h>

#include <unordered_map>

using namespace std;

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
		void Render(const Shader* shader);

		void Update();
		void SetAnimation(const char *animationName);
		void SetPlayback(bool value);
		bool TogglePlayback();

	private:
		void InitControlSkeleton();

		void UpdateGPUData();
		void UpdateAnimation();
		void UpdateAnimationNodesMapping();
		void UpdateJointTransform(SkeletalJoint * joint, float animationTime);

	private:
		uint nrBones;
		SkinnedMesh *skinnedMesh;
		unordered_map<string, SkeletalJoint*> skeletalJoints;
		vector<glm::mat4> boneTransform;

		SkeletalJoint *rootJoint;
		glm::mat4 rootTransform;
		glm::mat4 globalInvTransform;

		#ifdef ENGINE_DLL_EXPORTS
		aiAnimation *currentAnimation;
		#endif
		bool playbackState;
		bool controlState;
};