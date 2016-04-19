#include "SkeletalJoint.h"

#include <iostream>
using namespace std;

#include <include/gl.h>
#include <include/glm_utils.h>
#include <include/assimp_utils.h>
#include <Utils/OpenGL.h>

#include <Component/AABB.h>
#include <Component/Mesh.h>
#include <Component/Renderer.h>
#include <Component/Transform/Transform.h>
#include <Component/Transform/SkinningJointTransform.h>
#include <GPU/Shader.h>

#include <Rendering/DirectOpenGL.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/ResourceManager.h>


SkeletalJoint::SkeletalJoint(const char * name, uint jointID)
	: GameObject(name)
	, jointID(jointID)
{
	//SAFE_FREE(transform);
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
	//SAFE_FREE(transform);
	transform = new SkinningJointTransform();
	auto mesh = Manager::GetResource()->GetMesh("box");
	SetMesh(mesh);
	renderer->SetRenderingLayer(RenderingLayer::ON_TOP);
	boneOffset = joint.boneOffset;
	TPoseOffset = joint.TPoseOffset;
	pAnimationNode = joint.pAnimationNode;
}

SkeletalJoint::~SkeletalJoint()
{
}

void SkeletalJoint::Render(const Shader *shader) const
{
	GameObject::Render(shader);

	auto DirectGL = Manager::GetDirectGL();
	DirectGL->Use();
	DirectGL->SetDrawColor(200, 70, 50);

	RenderBones(shader);

	shader->Use();
}

void SkeletalJoint::RenderBones(const Shader * shader) const
{
	auto DirectGL = Manager::GetDirectGL();
	for (auto child : _children) {
		DirectGL->DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
	}
}

void SkeletalJoint::RenderDebug(const Shader * shader) const
{
	glDepthMask(GL_FALSE);

	GameObject::Render(shader);
	
	auto DirectGL = Manager::GetDirectGL();
	DirectGL->Use();
	DirectGL->SetDrawColor(255, 255, 0);

	RenderBones(shader);

	glDepthMask(GL_TRUE);
	shader->Use();
}

void SkeletalJoint::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void SkeletalJoint::UpdateSkeletonBindPose(const glm::mat4 & globalInverse)
{
	transform->ForceUpdate();
	UpdateTransformBindPose(globalInverse);
	for (auto child : _children) {
		((SkeletalJoint*)child)->UpdateSkeletonBindPose(globalInverse);
	}
}

void SkeletalJoint::UpdateTransformBindPose(const glm::mat4 &globalInverse)
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