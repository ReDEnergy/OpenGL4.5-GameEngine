#include "pch.h"

#include "SkeletalSystem.h"

#include <Windows.h>

#include <Kinect/KinectAPI.h>

#include <SkeletalSystem/SkeletalJoint.h>

SkeletalSystem::SkeletalSystem()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{
	FBO = new FrameBuffer();
	FBO->Generate(Engine::Window->resolution.x, Engine::Window->resolution.y, 1);

	jointShader = Manager::GetShader()->GetShader("simple");

	kinectJointMapping.resize(JointType_Count);
	recordKinectState = false;
	recordKinectRot = false;
	recordKinectHack = false;
}

SkeletalSystem::~SkeletalSystem() {
}

void SkeletalSystem::Init()
{
	Clear();

	const pugi::xml_document *doc = pugi::LoadXML("Config/SkeletalSystem.xml");

	GetJoints(doc->child("skelet").child("joints"));

	ROOT->transform->SetWorldPosition(glm::vec3(2, 1, 0));

	SAFE_FREE(doc);

}

void SkeletalSystem::Clear()
{
	joints.clear();
}

void SkeletalSystem::GetJoints(pugi::xml_node &node)
{
	if (ROOT)
		Manager::GetScene()->RemoveObject((GameObject*)ROOT);
	SAFE_FREE(ROOT);

	const char* rotations[3] = { "ox\0", "oy\0", "oz\0" };
	SkeletalJoint *SKJ;

	for (pugi::xml_node joint : node.children()) {

		// Get/create joint
		const char *name = joint.child_value("name");

		SKJ = (joints.find(name) != joints.end()) ? joints[name] : new SkeletalJoint(name);
		joints[name] = SKJ;

		uint kinectID = joint.child("kinectID").text().as_uint();
		kinectJointMapping[kinectID] = SKJ;
		SKJ->sensorTrackingID = kinectID;

		// Get Parent Joint
		pugi::xml_node parent = joint.child("parent");
		if (parent) {
			const char *parentName = parent.text().get();
			if (joints.find(parentName) == joints.end())
				joints[parentName] = new SkeletalJoint(parentName);

			joints[parentName]->AddChild((GameObject*) SKJ);
		}
		else {
			ROOT = SKJ;
		}

		// Joint position
		pugi::xml_node position = joint.child("pos");
		if (position) {
			glm::vec3 pos = glm::ExtractVector<glm::vec3>(position.text().get());
			SKJ->transform->SetWorldPosition(pos);
		}

		// Joint constraints relative to parent
		pugi::xml_node constraints = joint.child("constraints");

		if (constraints) {
			for (uint i = 0; i < 3; i++) {
				if (constraints.child(rotations[i]))
					SKJ->rotConstraint[i] = glm::ExtractVector<glm::ivec2>(constraints.child(rotations[i]).text().get());
			}
		}
	}

	Manager::GetScene()->AddObject((GameObject*)ROOT);

}

SkeletalJoint * SkeletalSystem::GetRoot() const
{
	return ROOT;
}

void SkeletalSystem::Render(Camera *camera) const
{
	if (!ROOT)
		return;

	FBO->Bind();

	jointShader->Use();

	camera->BindViewMatrix(jointShader->loc_view_matrix);
	camera->BindProjectionMatrix(jointShader->loc_projection_matrix);

	glLineWidth(4);

	ROOT->RenderSkelet(jointShader);

	FrameBuffer::Unbind();
}

void SkeletalSystem::Update(SkeletalTracking * tracking)
{
	if (!ROOT)
		return;

	if (recordKinectState) {
		ROOT->ReadSensorPositionState(tracking);
		ROOT->UpdateJointScale();
		recordKinectState = false;
	}

	if (recordKinectHack) {
		//RecordSkeletalPhysicState(tracking);
		ROOT->ReadSensorRotationState(tracking);
	}

	if (recordKinectRot) {
		ROOT->ReadSensorRotationState(tracking);
/*
		uint trackingID = 0;
		
		for (auto joint : kinectJointMapping) {
			if (joint) {
				JointOrientation &JO = tracking->jointRotations[trackingID];
				glm::quat rotQuat(JO.Orientation.w, JO.Orientation.x, JO.Orientation.y, JO.Orientation.z);
				joint->transform->SetWorldRotation(rotQuat);
			}
			trackingID++;
		}
*/		recordKinectRot = false;
	}
}

void SkeletalSystem::OnKeyPress(int key, int mods)
{
	if (mods != (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
		return;

	if (key == GLFW_KEY_L) {
		Init();
	}

	if (key == GLFW_KEY_P) {
		recordKinectState = true;
	}

	if (key == GLFW_KEY_I) {
		recordKinectRot = true;
	}

	// Mega HACK ---- WHY? WHY? WHY? WHY? WHY? WHY? 
	if (key == GLFW_KEY_H) {
		recordKinectHack = !recordKinectHack;
	}

}

void SkeletalSystem::RecordSkeletalPhysicState(SkeletalTracking * tracking)
{
	uint trackingID = 0;
	for (auto joint : kinectJointMapping) {
		if (joint) {
			CameraSpacePoint &pos = tracking->joints[trackingID].Position;
			joint->transform->SetWorldPosition(glm::vec3(pos.X, pos.Y, pos.Z) + glm::vec3(3, 15, 0));
		}
		trackingID++;
	}

	recordKinectState = false;
}

void SkeletalSystem::BindForComposition(GLenum DEPTH_TEXTURE_UNIT, GLenum CONTENT_TEXTURE_UNIT)
{
	FBO->BindTexture(0, CONTENT_TEXTURE_UNIT);
	FBO->BindDepthTexture(DEPTH_TEXTURE_UNIT);
}