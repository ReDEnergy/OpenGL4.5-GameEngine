﻿#include "pch.h"

#include "SkeletalSystem.h"

#include <Windows.h>

#include <Kinect/KinectAPI.h>

#include <SkeletalSystem/SkeletalJoint.h>
#include <SkeletalSystem/Transform/JointTransform.h>

SkeletalSystem::SkeletalSystem()
	: ObjectInput(InputGroup::IG_GAMEPLAY)
{
	FBO = new FrameBuffer();
	FBO->Generate(Engine::Window->resolution.x, Engine::Window->resolution.y, 1);

	jointShader = Manager::GetShader()->GetShader("simple");

	kinectJointMapping.resize(JointType_Count);
	recordKinectState = false;
	readKinectRot = false;
	readKinectPos = false;
	skeletonControl = Manager::GetResource()->GetGameObject("box");
	skeletonControl->transform->SetScale(glm::vec3(0.1f));
	skeletonControl->transform->SetWorldPosition(glm::vec3(1, 1.5, 2));

	readKinectPos = false;
	readKinectRot = false;

	SubscribeToEvent("reload-config");
}

SkeletalSystem::~SkeletalSystem() {
}

void SkeletalSystem::Init()
{
	Clear();

	const pugi::xml_document *doc = pugi::LoadXML("Config/SkeletalSystem.xml");

	GetJoints(doc->child("skelet").child("joints"));

	ROOT->transform->SetWorldPosition(glm::vec3(0, 0, 0));

	SAFE_FREE(doc);

}

void SkeletalSystem::Clear()
{
	joints.clear();
}

void SkeletalSystem::GetJoints(pugi::xml_node &node)
{
	if (ROOT)
		Manager::GetScene()->RemoveObject((GameObject*)ROOT, true);

	const char* rotations[3] = { "ox\0", "oy\0", "oz\0" };
	SkeletalJoint *SKJ;

	for (pugi::xml_node joint : node.children())
	{
		// Get/create joint
		const char *name = joint.child_value("name");

		SKJ = (joints.find(name) != joints.end()) ? joints[name] : new SkeletalJoint(name);
		joints[name] = SKJ;

		uint kinectID = joint.child("kinectID").text().as_uint();
		kinectJointMapping[kinectID] = SKJ;
		SKJ->SetJointID(kinectID);

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

		// TODO - use cone constraint
		// Joint constraints relative to parent
		pugi::xml_node constraints = joint.child("constraints");

		if (constraints)
		{
			auto dir = glm::ExtractVector<glm::vec3>(constraints.child_value("dir"));
			float angle = constraints.child("angle").text().as_float();
			static_cast<JointTransform*>(SKJ->transform)->SetConstraint(dir, angle);
		}
	}

	skeletonControl->RemoveChildren();
	skeletonControl->AddChild(ROOT);
	Manager::GetScene()->AddObject((GameObject*)skeletonControl);

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
	skeletonControl->Render(jointShader);

	FrameBuffer::Unbind();
}

void SkeletalSystem::Update(SkeletalTracking * tracking)
{
	if (!ROOT)
		return;

	bool state = false;
	if (recordKinectState) {
		ROOT->ReadSensorRotationState(tracking, true);
		ROOT->ReadSensorPositionState(tracking, true);
		state = true;
	}

	if (readKinectPos) {
		ROOT->ReadSensorPositionState(tracking);
		readKinectPos = false;
		state = true;
	}

	if (readKinectRot) {
		ROOT->ReadSensorRotationState(tracking);
		readKinectRot = false;
		state = true;
	}

	if (state) {
		CameraSpacePoint &hipsPos = tracking->joints[JointType_SpineBase].Position;
		ROOT->transform->SetLocalPosition(glm::vec3(hipsPos.X, hipsPos.Y, hipsPos.Z));
	}
}

void SkeletalSystem::OnKeyPress(int key, int mods)
{
	if (mods != (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
		return;

	if (key == GLFW_KEY_L) {
		Manager::GetEvent()->EmitSync("reload-config", nullptr);
	}

	if (key == GLFW_KEY_P) {
		readKinectPos = true;
	}

	if (key == GLFW_KEY_O) {
		readKinectRot = true;
	}

	if (key == GLFW_KEY_H) {
		recordKinectState = !recordKinectState;
	}

	if (key == GLFW_KEY_M) {
		auto x = dynamic_cast<JointTransform*>(Manager::GetPicker()->GetSelectedObject()->transform);
		if (x) {
			x->SetChildrenRotation(glm::AxisAngle(0, 1, 0, 45));
		}
	}

}

void SkeletalSystem::RecordSkeletalPhysicState(SkeletalTracking * tracking)
{
	uint trackingID = 0;
	glm::vec3 pos = ROOT->transform->GetWorldPosition();
	for (auto joint : kinectJointMapping) {
		if (joint) {
			CameraSpacePoint &pos = tracking->joints[trackingID].Position;
			joint->transform->SetWorldPosition(glm::vec3(pos.X, pos.Y, pos.Z));
		}
		trackingID++;
	}
	ROOT->transform->SetWorldPosition(pos);

	recordKinectState = false;
}

void SkeletalSystem::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("reload-config") == 0) {
		Init();
	}
}

void SkeletalSystem::BindForComposition(GLenum DEPTH_TEXTURE_UNIT, GLenum CONTENT_TEXTURE_UNIT)
{
	FBO->BindTexture(0, CONTENT_TEXTURE_UNIT);
	FBO->BindDepthTexture(DEPTH_TEXTURE_UNIT);
}