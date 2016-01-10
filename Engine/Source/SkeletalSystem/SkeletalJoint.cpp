#include "SkeletalJoint.h"

#include <iostream>
using namespace std;

#include <include/gl_utils.h>
#include <include/glm_utils.h>
#include <include/assimp_utils.h>
#include <Utils/OpenGL.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Renderer.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/SkinningJointTransform.h>
#include <GPU/Shader.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/ResourceManager.h>


SkeletalJoint::SkeletalJoint(const char * name, uint jointID)
	: GameObject(name)
	, jointID(jointID)
{
	SAFE_FREE(transform);
	transform = new SkinningJointTransform();
	auto mesh = Manager::GetResource()->GetMesh("box");
	SetName(name);
	SetMesh(mesh);
	renderer->SetRenderingLayer(RenderingLayer::ON_TOP);
}

SkeletalJoint::SkeletalJoint(const SkeletalJoint& joint)
	: GameObject(joint.GetName())
	, jointID(joint.jointID)
{
	SAFE_FREE(transform);
	transform = new SkinningJointTransform();
	auto mesh = Manager::GetResource()->GetMesh("box");
	SetMesh(mesh);
	renderer->SetRenderingLayer(RenderingLayer::ON_TOP);
	boneOffset = joint.boneOffset;
	TPoseOffset = joint.TPoseOffset;
}

SkeletalJoint::~SkeletalJoint()
{
}

void SkeletalJoint::Render(const Shader *shader) const
{
	GL_Utils::SetColor256(shader->loc_debug_color, 30, 185, 80);
	GameObject::Render(shader);
	RenderBones(shader);
}

void SkeletalJoint::RenderBones(const Shader * shader) const
{
	glLineWidth(2);
	GL_Utils::SetColor256(shader->loc_debug_color, 200, 70, 50);
	glm::mat4 scale = glm::scale(glm::mat4(1), transform->GetScale());
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(scale));
	for (auto child : _children) {
		RenderBone((SkeletalJoint*)child);
	}
}

void SkeletalJoint::RenderDebug(const Shader * shader) const
{
	glDepthMask(GL_FALSE);
	GL_Utils::SetColorUnit(shader->loc_debug_color, 1, 1, 0);
	GameObject::Render(shader);
	RenderBones(shader);
	glDepthMask(GL_TRUE);
}

void SkeletalJoint::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void SkeletalJoint::UpdateSkeletonTPose(const glm::mat4 & globalInverse)
{
	transform->ForceUpdate();
	UpdateTransformTPose(globalInverse);
	for (auto child : _children) {
		((SkeletalJoint*)child)->UpdateSkeletonTPose(globalInverse);
	}
}

void SkeletalJoint::UpdateTransformTPose(const glm::mat4 &globalInverse)
{
	if (_parent) {
		globalTransform = ((SkeletalJoint*)_parent)->globalTransform;
	}
	globalTransform *= TPoseOffset * transform->GetModel();
	((SkinningJointTransform*)transform)->SetFakeModel(globalTransform);
	finalTransformation = globalInverse * globalTransform * boneOffset;

	// TODO investigate with other skinned meshes if globalInverse is required or not.... 
	// it shouldn't as long as the asset is exported corectly - vertical oriented
	//finalTransformation = globalInverse * globalTransform * boneOffset;
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
	((SkinningJointTransform*)transform)->SetFakeModel(globalTransform);
	finalTransformation = globalInverse * globalTransform * boneOffset;
}

uint SkeletalJoint::GetJointID() const
{
	return jointID;
}

void SkeletalJoint::LogDebugInfo() const
{
	cout << "[BoneOffeset]" << endl;
	for (int i = 0; i < 4; i++)
		cout << boneOffset[i] << endl;

	cout << "[TPose]" << endl;
	for (int i = 0; i < 4; i++)
		cout << TPoseOffset[i] << endl;
}

void SkeletalJoint::RenderBone(SkeletalJoint *child) const
{
	OpenGL::DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
}