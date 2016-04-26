#pragma once
#include <string>
#include <vector>

#include <include/dll_export.h>

#include <Core/GameObject.h>

typedef unsigned int uint;

struct aiNodeAnim;

class DLLExport SkeletalJoint
	: public GameObject
{
	public:
		SkeletalJoint(const char* name, uint jointID = 0);
		SkeletalJoint(const SkeletalJoint &joint);
		~SkeletalJoint();

		void Render(const Shader *shader) const;
		void RenderBones(const Shader *shader) const;
		void RenderDebug(const Shader *shader) const;

		void RenderForPicking(const Shader * shader) const;

		void UpdateSkeleton();

		uint GetJointID() const;

	public:
		glm::vec3 scale;
		glm::mat4 TPoseOffset;
		glm::mat4 boneOffset;
		//glm::mat4 globalTransform;
		glm::mat4 finalTransformation;
		const aiNodeAnim* pAnimationNode;

	private:
		glm::ivec2 rotConstraint[3];
		uint jointID;
};
