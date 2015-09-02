#pragma once
#include <string>
#include <vector>

#include <include/dll_export.h>

#include <Core/GameObject.h>

using namespace std;

typedef unsigned int uint;

struct aiNodeAnim;

class DLLExport SkeletalJoint
	: public GameObject
{
	public:
		SkeletalJoint(const char* name, uint jointID = 0);
		~SkeletalJoint();

		void Render(const Shader *shader) const;
		void RenderSkelet(const Shader * shader) const;
		void RenderBones(const Shader *shader) const;
		void RenderDebug(const Shader *shader) const;

		void RenderForPicking(const Shader * shader) const;

		void UpdateSkeleton(const glm::mat4 &globalInverse);
		void UpdateTransform(const glm::mat4 &globalInverse);

		uint GetJointID();

	private:
		void RenderBone(SkeletalJoint * child) const;

	public:
		glm::mat4 boneOffset;
		glm::mat4 globalTransform;
		glm::mat4 finalTransformation;
		const aiNodeAnim* pAnimationNode;

	private:
		glm::ivec2 rotConstraint[3];
		uint jointID;
};
