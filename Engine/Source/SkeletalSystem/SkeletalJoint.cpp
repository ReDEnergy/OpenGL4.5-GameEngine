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
#include <Component/Transform/LimitedTransform.h>
#include <GPU/Shader.h>

#include <Rendering/DirectOpenGL.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/ResourceManager.h>


SkeletalJoint::SkeletalJoint(const char * name, uint jointID)
	: GameObject(name)
	, jointID(jointID)
{
	Init();
}

SkeletalJoint::SkeletalJoint(const SkeletalJoint& joint)
	: GameObject(joint.GetName())
	, jointID(joint.jointID)
{
	Init();
	boneOffset = joint.boneOffset;
	pAnimationNode = joint.pAnimationNode;
}

SkeletalJoint::~SkeletalJoint()
{
}

void SkeletalJoint::Init()
{
	SAFE_FREE(transform);
	transform = new LimitedTransform();
	auto mesh = Manager::GetResource()->GetMesh("box");
	SetMesh(mesh);
	renderer->SetRenderingLayer(RenderingLayer::ON_TOP);
}

void SkeletalJoint::SetBoneOffset(glm::mat4 offset)
{
	boneOffset = std::move(offset);
}

void SkeletalJoint::UpdateBoneTransform()
{
	boneTransformation = transform->GetModel() * boneOffset;
}

const glm::mat4& SkeletalJoint::GetBoneTransform() const
{
	return boneTransformation;
}

void SkeletalJoint::Render(const Shader *shader) const
{
	// TODO - improve performance
	// don't switch between normal rendering and DirectGL
	// First render the joint hierarchy using instantiation
	// Render bone hierarchy one DiretGL pass

	GameObject::Render(shader);

	auto DirectGL = Manager::GetDirectGL();
	DirectGL->UseDefaultShader();
	DirectGL->BindActiveViewProj();
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
	DirectGL->UseDefaultShader();
	DirectGL->SetDrawColor(255, 255, 0);

	RenderBones(shader);

	glDepthMask(GL_TRUE);
	shader->Use();
}

void SkeletalJoint::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void SkeletalJoint::UpdateSkeleton()
{
	boneTransformation = transform->GetModel() * boneOffset;
	for (auto child : _children) {
		((SkeletalJoint*)child)->UpdateSkeleton();
	}
}

uint SkeletalJoint::GetJointID() const
{
	return jointID;
}

float SkeletalJoint::GetBoneLength() const
{
	return boneLength;
}

void SkeletalJoint::ComputeBoneLength()
{
	boneLength = _parent ? DistTo(*_parent) : 0;
}

void SkeletalJoint::FindAnimationNode(const aiAnimation *animation)
{
	pAnimationNode = assimp::FindNodeAnim(animation, name);
}

const aiNodeAnim * SkeletalJoint::GetAssimpAnimationNode() const
{
	return pAnimationNode;
}
