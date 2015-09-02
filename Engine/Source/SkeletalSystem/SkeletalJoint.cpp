#include "SkeletalJoint.h"

#include <include/gl_utils.h>
#include <include/assimp_utils.h>
#include <Utils/OpenGL.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/JointTransform.h>
#include <GPU/Shader.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/ResourceManager.h>


SkeletalJoint::SkeletalJoint(const char * name, uint jointID)
	: GameObject(name)
	, jointID(jointID)
{
	SAFE_FREE(transform);
	transform = new JointTransform();
}

SkeletalJoint::~SkeletalJoint()
{
}

void SkeletalJoint::Render(const Shader *shader) const
{
	if (!mesh) return;
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));
	mesh->Render(shader);
	for (auto child : _children) {
		child->Render(shader);
	}
}

void SkeletalJoint::RenderSkelet(const Shader *shader) const
{
	GL_Utils::SetColor256(shader->loc_debug_color, 30, 185, 80);
	GameObject::Render(shader);

	GL_Utils::SetColor256(shader->loc_debug_color, 200, 70, 50);
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
	RenderBones(shader);
}

void SkeletalJoint::RenderBones(const Shader * shader) const
{
	for (auto child : _children) {
		RenderBone((SkeletalJoint*)child);
		((SkeletalJoint*)child)->RenderBones(shader);
	}
}

void SkeletalJoint::RenderDebug(const Shader * shader) const
{
	if (mesh) {
		mesh->RenderDebug(shader);
	}
}

void SkeletalJoint::RenderForPicking(const Shader * shader) const
{
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	GameObject::RenderForPicking(shader);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
}

void SkeletalJoint::UpdateSkeleton(const glm::mat4 & globalInverse)
{
	transform->ForceUpdate();
	UpdateTransform(globalInverse);
	for (auto child : _children) {
		((SkeletalJoint*)child)->UpdateSkeleton(globalInverse);
	}
}

void SkeletalJoint::UpdateTransform(const glm::mat4 &globalInverse)
{
	if (_parent) {
		globalTransform = ((SkeletalJoint*)_parent)->globalTransform;
	}
	globalTransform *= transform->GetModel();
	((JointTransform*)transform)->SetFakeModel(this->globalTransform);
	finalTransformation = globalInverse * globalTransform * boneOffset;
}

uint SkeletalJoint::GetJointID()
{
	return jointID;
}

void SkeletalJoint::RenderBone(SkeletalJoint *child) const
{
	OpenGL::DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
}