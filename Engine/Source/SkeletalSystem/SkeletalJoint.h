#pragma once
#include <string>
#include <vector>

#include <include/dll_export.h>

#include <Core/GameObject.h>

typedef unsigned int uint;

struct aiNodeAnim;
struct aiAnimation;

class DLLExport SkeletalJoint
	: public GameObject
{
	public:
		SkeletalJoint(const char* name, uint jointID = 0);
		SkeletalJoint(const SkeletalJoint &joint);
		~SkeletalJoint();

		void SetBoneOffset(glm::mat4 offset);
		void UpdateBoneTransform();
		const glm::mat4& GetBoneTransform() const;

		void Render(const Shader *shader) const;
		void RenderBones(const Shader *shader) const;
		void RenderDebug(const Shader *shader) const;

		void RenderForPicking(const Shader * shader) const;

		void UpdateSkeleton();

		uint GetJointID() const;
		float GetBoneLength() const;
		void ComputeBoneLength();

		void FindAnimationNode(const aiAnimation *animation);
		const aiNodeAnim* GetAssimpAnimationNode() const;

	private:
		void Init();

	private:
		float boneLength;
		glm::mat4 boneOffset;
		glm::mat4 boneTransformation;
		const aiNodeAnim* pAnimationNode;

	private:
		glm::ivec2 rotConstraint[3];
		uint jointID;
};
