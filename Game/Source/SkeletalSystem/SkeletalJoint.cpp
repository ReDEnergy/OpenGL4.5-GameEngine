#include "pch.h"

#include "SkeletalJoint.h"

#include <Kinect/KinectAPI.h>

SkeletalJoint::SkeletalJoint(const char * name)
	: GameObject(name)
{

	mesh = Manager::GetResource()->GetMesh("box");
	transform->SetScale(glm::vec3(0.05f));
}

SkeletalJoint::~SkeletalJoint()
{
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

void SkeletalJoint::ReadSensorPositionState(SkeletalTracking * tracking)
{
	glm::vec3 offset(3, 2, 0);
	if (sensorTrackingID < JointType_Count && JointType_Count) {
		CameraSpacePoint &pos = tracking->joints[sensorTrackingID].Position;
		transform->SetWorldPosition(glm::vec3(pos.X, pos.Y, pos.Z) + offset);
	}

	for (auto child : _children) {
		((SkeletalJoint*)child)->ReadSensorPositionState(tracking);
	}
}

void SkeletalJoint::UpdateJointScale()
{
	if (_parent) {
		float scale = transform->DistanceTo(_parent->transform) / 5;
		scale = min(0.05f, scale);
		transform->SetScale(glm::vec3(scale));
	}
	for (auto child : _children) {
		((SkeletalJoint*)child)->UpdateJointScale();
	}
}

void SkeletalJoint::ReadSensorRotationState(SkeletalTracking * tracking)
{
	if (sensorTrackingID < JointType_Count) {
		JointOrientation &JO = tracking->jointRotations[sensorTrackingID];
		glm::quat rotQuat(JO.Orientation.w, JO.Orientation.x, JO.Orientation.y, JO.Orientation.z);
		transform->SetWorldRotation(rotQuat);
	}

	for (auto child : _children) {
		((SkeletalJoint*)child)->ReadSensorRotationState(tracking);
	}

}

void SkeletalJoint::RenderBone(SkeletalJoint *child) const
{
	OpenGL::DrawLine(transform->GetWorldPosition(), child->transform->GetWorldPosition());
}
