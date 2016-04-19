#include <pch.h>

#include <iostream>

#include <include/gl_utils.h>

#include "GSkeletalJoint.h"

#include <Kinect/KinectAPI.h>
#include <Kinect/Smoothing/ExponentialJoint.h>
#include <Kinect/Smoothing/ExponentialRotation.h>

#include <SkeletalSystem/Transform/JointTransform.h>

using namespace std;

GSkeletalJoint::GSkeletalJoint(const char * name)
	: GameObject(name)
{
	SetName(name);
	SetMesh(Manager::GetResource()->GetMesh("box"));

	//SAFE_FREE(transform);
	//transform = new JointTransform();
	transform->SetScale(glm::vec3(0.01f));

	boneOrientation = new Transform();
	constrained = new GameObject("constraint");
	constrained->SetMesh(Manager::GetResource()->GetMesh("box"));

	//Manager::GetScene()->AddObject(constrained);
}

GSkeletalJoint::~GSkeletalJoint()
{
}

// ****************************************************************************
// Renderes the whole skeleton

void GSkeletalJoint::Render(const Shader *shader) const
{
	//transform->SetScale(glm::vec3(0.01f));
	GameObject::Render(shader);
	//transform->SetScale(glm::vec3(1));

	auto DirectGL = Manager::GetDirectGL();
	DirectGL->Use();
	DirectGL->SetLineWidth(3);
	DirectGL->SetDrawColor(200, 240, 50);

	RenderBones(shader);

	shader->Use();
}

void GSkeletalJoint::RenderForPicking(const Shader * shader) const
{
	//transform->SetScale(glm::vec3(0.01f));
	GameObject::RenderForPicking(shader);
	//transform->SetScale(glm::vec3(1));
}

void GSkeletalJoint::RenderBones(const Shader * shader) const
{
	auto DirectGL = Manager::GetDirectGL();
	auto wpos = transform->GetWorldPosition();
	for (auto child : _children) {
		DirectGL->DrawLine(wpos, child->transform->GetWorldPosition());
		((GSkeletalJoint*)child)->RenderBones(shader);
	}
}

void GSkeletalJoint::ReadSensorPositionState(SkeletalTracking * tracking, bool ignoreNonTracked)
{
	auto trackingState = tracking->joints[sensorJointID].TrackingState;
	if (!ignoreNonTracked || trackingState == TrackingState::TrackingState_Tracked)
	{
		//CameraSpacePoint &pos = tracking->joints[sensorJointID].Position;
		//transform->SetWorldPosition(glm::vec3(pos.X, pos.Y, pos.Z));
		transform->SetWorldPosition(tracking->expJoints[sensorJointID]->filteredPosition);
	}

	for (auto child : _children) {
		((GSkeletalJoint*)child)->ReadSensorPositionState(tracking);
	}
}

void GSkeletalJoint::SetJointID(uint jointID)
{
	sensorJointID = jointID;
}

void GSkeletalJoint::Update()
{
	GameObject::Update();

	auto joint = dynamic_cast<GSkeletalJoint*>(this);
	if (!joint) return;

	if (transform->GetMotionState())
	{
		return;
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
		
		//auto dotv = glm::dot(boneOrientation->GetLocalOYVector(), transform->GetLocalOYVector());
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

uint GSkeletalJoint::GetJointID() const
{
	return sensorJointID;
}

void GSkeletalJoint::LogDebugInfo()
{
	cout << "Awesome" << endl;
}

void GSkeletalJoint::ReadSensorRotationState(SkeletalTracking * tracking, bool ignoreNonTracked)
{
	auto trackingState = tracking->joints[sensorJointID].TrackingState;
	if (!ignoreNonTracked || trackingState == TrackingState::TrackingState_Tracked)
	{
		transform->SetWorldRotation(tracking->expRotations[sensorJointID]->filteredRotation);
		boneOrientation->Copy(*transform);
	}

	for (auto child : _children) {
		auto joint = dynamic_cast<GSkeletalJoint*>(child);
		if (joint) {
			joint->ReadSensorRotationState(tracking);
		}
	}
} 

void GSkeletalJoint::RenderBone(GSkeletalJoint *child) const
{
	auto DirectGL = Manager::GetDirectGL();
	DirectGL->DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
}

void GSkeletalJoint::RenderConstraint(const Shader *shader) const
{
	float scale = 5;

	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(boneOrientation->GetModel()));
	auto T = static_cast<JointTransform*>(transform);

	//glLineWidth(2);

	///////////////////////////////////////////////////////////////////////////
	// Draw Constraing line
	auto dir = T->GetConstraintDirection() * (scale + 1);

	GL_Utils::SetColor256(shader->loc_debug_color, 30, 240, 30);
	//OpenGL::DrawLine(dir);

	///////////////////////////////////////////////////////////////////////////
	// Draw Parent coordinate system
	auto up = glm::vec3(0, 1, 0) * scale;

	GL_Utils::SetColor256(shader->loc_debug_color, 30, 30, 240);
	//OpenGL::DrawLine(glm::vec3(0), up);
	//OpenGL::DrawLine(up, up - glm::vec3(1, 1, 0));
	//OpenGL::DrawLine(up, up - glm::vec3(-1, 1, 0));

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

	//GL_Utils::SetColor256(shader->loc_debug_color, 50, 50, 50);
	//OpenGL::DrawCircle(glm::vec3(0, coneHeight, 0), radius, 32);
	//OpenGL::DrawCone(glm::vec3(0, coneHeight, 0), radius, -coneHeight, 32);


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