#include <pch.h>
#include <fstream>

#include "SkeletalSystem.h"

#include <Windows.h>

#include <Kinect/KinectAPI.h>

#include <SkeletalSystem/KinectAvatar.h>
#include <SkeletalSystem/GSkeletalJoint.h>
#include <SkeletalSystem/Transform/JointTransform.h>

#include <SkeletalSystem/AnimationSystem.h>

#include <include/gl.h>

using namespace std;

SkeletalSystem::SkeletalSystem()
{
	auto W = WindowManager::GetDefaultWindow();
	AttachTo(W);

#ifdef KINECT_SENSOR
	kinectJointMapping.resize(JointType_Count);
#endif

	recordKinectState = false;
	readKinectRot = false;
	readKinectPos = false;
	skeletonControl = Manager::GetResource()->GetGameObject("box");
	skeletonControl->SetName("SkeletonControl");
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

	skeletonControl->RemoveChildren();
	skeletonControl->AddChild(ROOT);
	Manager::GetScene()->AddObject((GameObject*)skeletonControl);

	//avatar = new KinectAvatar("");

	Manager::GetScene()->OnPostRender([this](Camera &camera) {
		this->ROOT->RenderSkeleton(camera);
	});
}

void SkeletalSystem::Clear()
{
	joints.clear();
}

const unordered_map<string, GSkeletalJoint*>& SkeletalSystem::GetSkeletalJoints() const
{
	return joints;
}

const std::vector<GSkeletalJoint*>& SkeletalSystem::GetKinectJointMapping() const
{
	return kinectJointMapping;
}

void SkeletalSystem::GetJoints(pugi::xml_node &node)
{
	if (ROOT)
		Manager::GetScene()->RemoveObject((GameObject*)ROOT, true);

	const char* rotations[3] = { "ox\0", "oy\0", "oz\0" };
	GSkeletalJoint *SKJ;

	for (pugi::xml_node joint : node.children())
	{
		// Get/create joint
		const char *name = joint.child_value("name");

		SKJ = (joints.find(name) != joints.end()) ? joints[name] : new GSkeletalJoint(name);
		joints[name] = SKJ;

		uint kinectID = joint.child("kinectID").text().as_uint();
		kinectJointMapping[kinectID] = SKJ;
		SKJ->SetJointID(kinectID);

		// Get Parent Joint
		pugi::xml_node parent = joint.child("parent");
		if (parent) {
			const char *parentName = parent.text().get();
			if (joints.find(parentName) == joints.end())
				joints[parentName] = new GSkeletalJoint(parentName);

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
}

void SkeletalSystem::Update()
{
#ifdef KINECT_SENSOR

	auto tracking = SINGLETON(KinectSkeletalTracking);

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

	if (state) {
		CameraSpacePoint &hipsPos = tracking->joints[JointType_SpineBase].Position;
		ROOT->transform->SetLocalPosition(glm::vec3(hipsPos.X, hipsPos.Y, hipsPos.Z));
	}
#endif
}

void SkeletalSystem::OnKeyPress(int key, int mods)
{
	if (mods != (GLFW_MOD_CONTROL | GLFW_MOD_SHIFT))
		return;

	//if (key == GLFW_KEY_L) {
	//	Manager::GetEvent()->EmitSync("reload-config", nullptr);
	//}

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

#ifdef KINECT_SENSOR
void SkeletalSystem::RecordSkeletalPhysicState(KinectSkeletalTracking * tracking)
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
#endif

void SkeletalSystem::OnEvent(const string& eventID, void * data)
{
	if (eventID.compare("reload-config") == 0) {
		Init();
	}
}