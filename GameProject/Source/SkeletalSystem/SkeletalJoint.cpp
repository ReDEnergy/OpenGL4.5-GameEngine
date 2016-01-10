#include "pch.h"

#include <iostream>

#include "SkeletalJoint.h"

#include <Kinect/KinectAPI.h>
#include <Kinect/Smoothing/ExponentialJoint.h>
#include <Kinect/Smoothing/ExponentialRotation.h>

#include <SkeletalSystem/Transform/JointTransform.h>

using namespace std;

SkeletalJoint::SkeletalJoint(const char * name)
	: GameObject(name)
{
	SetName(name);
	SetMesh(Manager::GetResource()->GetMesh("box"));
	SAFE_FREE(transform);
	transform = new JointTransform();
	transform->SetScale(glm::vec3(0.02f));
	boneOrientation = new Transform();
	constrained = new GameObject("constraint");
	constrained->SetMesh(Manager::GetResource()->GetMesh("box"));
	//Manager::GetScene()->AddObject(constrained);
}

SkeletalJoint::~SkeletalJoint()
{
}

// ****************************************************************************
// ATTENTION !!
// This function will render the whole skeleton

void SkeletalJoint::Render(const Shader *shader) const
{
	GameObject::Render(shader);

	RenderConstraint(shader);

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));

	RenderBones(shader);
}

void SkeletalJoint::RenderBones(const Shader * shader) const
{
	GL_Utils::SetColor256(shader->loc_debug_color, 200, 240, 50);
	for (auto child : _children) {
		RenderBone((SkeletalJoint*)child);
		((SkeletalJoint*)child)->RenderBones(shader);
	}
}

void SkeletalJoint::ReadSensorPositionState(SkeletalTracking * tracking, bool ignoreNonTracked)
{
	auto trackingState = tracking->joints[sensorJointID].TrackingState;
	if (!ignoreNonTracked || trackingState == TrackingState::TrackingState_Tracked)
	{
		//CameraSpacePoint &pos = tracking->joints[sensorJointID].Position;
		//transform->SetWorldPosition(glm::vec3(pos.X, pos.Y, pos.Z));
		transform->SetWorldPosition(tracking->expJoints[sensorJointID]->filteredPosition);
	}

	for (auto child : _children) {
		((SkeletalJoint*)child)->ReadSensorPositionState(tracking);
	}
}

void SkeletalJoint::SetJointID(uint jointID)
{
	sensorJointID = jointID;
}

void SkeletalJoint::Update()
{
	GameObject::Update();

	if (transform->GetMotionState())
	{
		auto constraintT = constrained->transform;
		auto controlledObject = Manager::GetPicker()->GetSelectedObject();

		if (transform->GetMotionState() && this != controlledObject) {
			boneOrientation->Copy(*transform);
			constraintT->SetWorldPosition(transform->GetWorldPosition());
			constraintT->SetScale(transform->GetScale());
		}

		if ((this != controlledObject) || (GetNumberOfChildren() == 0)) {
			return;
		}

		// Apply constraints only if it has a child
		auto children = GetChildren();
		auto childT = static_cast<Transform*> (children.front()->transform);
		
		////auto dotv = glm::dot(boneOrientation->GetLocalOYVector(), transform->GetLocalOYVector());
		auto boneDir = glm::normalize(childT->GetWorldPosition() - transform->GetWorldPosition());
		auto dotv = glm::dot(boneOrientation->GetLocalOYVector(), boneDir);
		auto T = static_cast<JointTransform*>(transform);
		float radians = float(RADIANS(T->GetConstraintAngle() / 2));
		auto cValue = float(acos(dotv)) / radians;

		float ammount = (cValue - 1) / cValue;
		if (cValue > 1)
		{
			auto invRot = glm::inverse(childT->GetRelativeRotation());
			auto value = glm::slerp(glm::quat(), invRot, ammount);
			constraintT->SetWorldRotation(transform->GetWorldRotation() * childT->GetRelativeRotation() * value);
			constraintT->SetWorldPosition(transform->GetWorldPosition() + glm::rotate(constraintT->GetWorldRotation(), glm::vec3(0, 1, 0)) * glm::length(childT->GetLocalPosition()));
			childT->SetWorldRotation(constraintT->GetWorldRotation());
			childT->SetWorldPosition(constraintT->GetWorldPosition());
		}

		//cout << "------------------------------------------" << endl;
		//cout << GetName() << " => [DOT]: " << dotv << "\t[ANGLE] : " << acos(dotv) * TO_DEGREES << "\t[MAX] : " << radians * TO_DEGREES << endl;
		//cout << "[RADIANS] : " << acos(dotv) << "\t [MAX RADIANS] : " << radians << "\t [SLERP] : " << ammount << "\t" << cValue - 1 << endl;
		//auto rot = child->transform->GetRelativeRotation();
		//auto deg = glm::eulerAngles(rot);
		//cout << "[RELATIVE ROT] : " << deg << endl;
		//cout << endl;

		for (auto child : children) {
			child->Update();
		}

	}
}

void SkeletalJoint::LogDebugInfo()
{
	cout << "Awesome" << endl;
}

void SkeletalJoint::ReadSensorRotationState(SkeletalTracking * tracking, bool ignoreNonTracked)
{
	auto trackingState = tracking->joints[sensorJointID].TrackingState;
	if (!ignoreNonTracked || trackingState == TrackingState::TrackingState_Tracked)
	{
		transform->SetWorldRotation(tracking->expRotations[sensorJointID]->filteredRotation);
		boneOrientation->Copy(*transform);
	}

	for (auto child : _children) {
		((SkeletalJoint*)child)->ReadSensorRotationState(tracking);
	}
} 

void SkeletalJoint::RenderBone(SkeletalJoint *child) const
{
	OpenGL::DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
}

void SkeletalJoint::RenderConstraint(const Shader *shader) const
{
	float scale = 5;

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(boneOrientation->GetModel()));
	auto T = static_cast<JointTransform*>(transform);

	//glLineWidth(2);

	///////////////////////////////////////////////////////////////////////////
	// Draw Constraing line
	auto dir = T->GetConstraintDirection() * (scale + 1);

	GL_Utils::SetColor256(shader->loc_debug_color, 30, 240, 30);
	OpenGL::DrawLine(dir);

	///////////////////////////////////////////////////////////////////////////
	// Draw Parent coordinate system
	auto up = glm::vec3(0, 1, 0) * scale;

	GL_Utils::SetColor256(shader->loc_debug_color, 30, 30, 240);
	OpenGL::DrawLine(glm::vec3(0), up);
	OpenGL::DrawLine(up, up - glm::vec3(1, 1, 0));
	OpenGL::DrawLine(up, up - glm::vec3(-1, 1, 0));

	///////////////////////////////////////////////////////////////////////////
	// Draw Constraint Cone

	float radius = 2;
	float coneHeight = 2;
	auto coneAngle = T->GetConstraintAngle() / 2;
	if (coneAngle != 90) {
		coneHeight = radius / float(tan(RADIANS(coneAngle)));
	}
	if (coneHeight > radius) {
		coneHeight /= 2;
		radius /= 2;
	}

	GL_Utils::SetColor256(shader->loc_debug_color, 50, 50, 50);
	OpenGL::DrawCircle(glm::vec3(0, coneHeight, 0), radius, 32);
	OpenGL::DrawCone(glm::vec3(0, coneHeight, 0), radius, -coneHeight, 32);


	///////////////////////////////////////////////////////////////////////////
	// Draw Computed Constrained 
	//if (Manager::GetPicker()->GetSelectedObject() == this)
	//{
	//	float scale = 10;
	//	auto up = glm::vec3(0, 1, 0) * scale;
	//	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(constrained->transform->GetModel()));
	//	GL_Utils::SetColor256(shader->loc_debug_color, 242, 127, 12);
	//	OpenGL::DrawLine(up);
	//	OpenGL::DrawLine(up, up - glm::vec3(1, 1, 0));
	//	OpenGL::DrawLine(up, up - glm::vec3(-1, 1, 0));
	//}
}