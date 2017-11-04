#include <pch.h>
#include "KinectAvatar.h"

#include <include/gl.h>
#include <include/math.h>
#include <include/assimp_utils.h>

#include <SkeletalSystem/SkeletalSystem.h>
#include <SkeletalSystem/GSkeletalJoint.h>

using namespace std;

KinectAvatar::KinectAvatar(const char* meshName)
	: GameObject("KinectAvatar")
{
	motionScale = 0.01f;
	avatarScale = 0.01f;
	playMotion = false;
	scaleSkeleton = false;
	AttachTo(WindowManager::GetDefaultWindow());

	auto mesh = Manager::GetResource()->GetMesh("Atienza");
	if (!mesh) {
		return;
	}
	SetMesh(mesh);
	animation = new AnimationController();
	animation->Setup((SkinnedMesh*)mesh);
	animation->SetAnimation((uint)0);
	Manager::GetScene()->AddObject(this);

	//ReadBVH();

	//auto SkeletSys = SINGLETON(SkeletalSystem);
	//joints = SkeletSys->GetBVJoints();
	//motionFrames = SkeletSys->GetMotionData();

	boneTransform.resize(joints.size());
	for (auto joint : joints) {
		auto jointName = joint->GetName();
		auto it = animation->skeletalJoints.find(jointName);
		if (it != animation->skeletalJoints.end()) {
			auto jointID = it->second->GetJointID();
			joint->SetJointID(jointID);
		}
		else {
			std::cout << "[KinectAvatar] " << "No mapping for joint [" << jointName << "]!" << std::endl;
		}
	}

	transform->SetWorldPosition(glm::vec3(0, 0, 0));
	transform->SetScale(glm::vec3(avatarScale));
}

KinectAvatar::~KinectAvatar()
{
}

void KinectAvatar::ReadBVH()
{
	ifstream F;
	F.open("Resources/Models/Characters/Atienza/Atienza.bvh");

	GameObject *BVHhandle = Manager::GetResource()->GetGameObject("box");
	BVHhandle->SetName("BVH Handle");
	BVHhandle->transform->SetScale(glm::vec3(0.05f));
	Manager::GetScene()->AddObject(BVHhandle);

	joints.clear();

	string token;
	F >> token;

	if (token.compare("HIERARCHY") == 0) {

		// Read Node token and name
		F >> token;

		if (token.compare("ROOT") == 0) {
			ReadBVHJoint(F, BVHhandle);
		}
	}

	BVHhandle->transform->SetWorldPosition(glm::vec3(-2, 1.5f, 2));

	F >> token;
	if (token.compare("MOTION") == 0) {
		string info;
		F >> info >> totalFrames;
		F >> info >> info >> frameTime;

		motionFrames = new vector<glm::vec3>[totalFrames];

		glm::vec3 rotation, offset;
		auto nrJoints = joints.size();

		for (uint frameID = 0; frameID < totalFrames; frameID++) {
			F >> offset.z >> offset.y >> offset.x;

			motionFrames[frameID].push_back(offset * motionScale);

			for (size_t i = 0; i < nrJoints; i++) {
				F >> rotation.z >> rotation.y >> rotation.x;
				motionFrames[frameID].push_back(rotation);
			}
		}
	}
}

void KinectAvatar::ReadBVHJoint(ifstream &F, GameObject* parent)
{
	string skip, token, jointName;
	glm::vec3 offset;
	char line[256];

	// Read name
	F >> jointName;

	// An oppening brace should follow "{"
	F >> skip;

	// Read OFFSET
	F >> token >> offset.x >> offset.y >> offset.z;

	// Read CHANNELS
	F.getline(line, 256);
	F.getline(line, 256);

	auto joint = new GSkeletalJoint(jointName.c_str());
	parent->AddChild(joint);
	joint->transform->SetLocalPosition(offset * motionScale);
	//joint->transform->SetScale(glm::vec3(0.01f));
	joints.push_back(joint);

	while (1) {
		// Read name
		F >> token;

		if (token.compare("JOINT") == 0) {
			ReadBVHJoint(F, joint);
		}

		if (token.compare("End") == 0) {
			F.getline(line, 256);
			F.getline(line, 256);
			F.getline(line, 256);
			F.getline(line, 256);
		}

		if (token.compare("}") == 0) {
			break;
		}
	}
}

void KinectAvatar::Update()
{
	//GameObject::Update();

	if (playMotion)
	{
		// TODO - perform interpolation without timeout

		if (Engine::GetElapsedTime() - lastMotionUpdate < frameTime)
			return;

		lastMotionUpdate = Engine::GetElapsedTime();

		frameID++;
		frameID = frameID % totalFrames;

	}

	auto rootJoint = animation->rootJoint;
	float atime = 0;
	auto ttime = (totalFrames - 1) * frameTime;

	glm::vec3 offset(0, 0, -1);
	joints[0]->transform->SetLocalPosition(motionFrames[frameID][0] + offset);

	auto globalInvTransform = ((SkinnedMesh*)GetMesh())->GetGlobalInverse();
	auto rootTransform = ((SkinnedMesh*)GetMesh())->GetRootTransform();

	auto transf = rootTransform * globalInvTransform;

	for (uint i = 0; i < joints.size(); i++) {
		if (playMotion)
			joints[i]->transform->SetRelativeRotation(glm::quat(motionFrames[frameID][i + 1] * TO_RADIANS));

		auto it = animation->skeletalJoints.find(joints[i]->GetName());
		if (it != animation->skeletalJoints.end())
		{
			//auto boneOffset = it->second->boneOffset;
			//auto model = joints[i]->transform->GetModel();

			//auto relativePos = joints[i]->transform->GetLocalPosition();
			//auto relativeRot = joints[i]->transform->GetRelativeRotation();

			//// Interpolate transformation between 2 keyframes
			//glm::vec3 position = assimp::CalcInterpolatedPosition(it->second->pAnimationNode, atime, ttime);
			//glm::quat rotation = assimp::CalcInterpolatedRotation(it->second->pAnimationNode, atime, ttime);
			//glm::vec3 scale = assimp::CalcInterpolatedScaling(it->second->pAnimationNode, atime, ttime);

			auto id = joints[i]->GetJointID();
			boneTransform[id] = it->second->GetBoneTransform();

		}
	}
}

void KinectAvatar::Render(const Shader * shader) const
{
	glUniformMatrix4fv(shader->loc_bones, static_cast<unsigned int>(joints.size()), GL_FALSE, glm::value_ptr(boneTransform[0]));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(transform->GetModel()));
	if (meshRenderer) {
		renderer->Use();
		meshRenderer->Render();
	}
}

void KinectAvatar::OnKeyPress(int key, int mods)
{
	if (key == GLFW_KEY_8) {
		playMotion = !playMotion;
	}

	if (key == GLFW_KEY_K) {
		scaleSkeleton = !scaleSkeleton;
		float scale = scaleSkeleton ? 2.0f : 0.5f;
		for (uint i = 0; i < joints.size(); i++) {
			auto position = joints[i]->transform->GetWorldPosition();
			joints[i]->transform->SetWorldPosition(position * scale);
		}
	}

	if (key == GLFW_KEY_9) {
		for (uint i = 0; i < joints.size(); i++) {
			joints[i]->transform->SetRelativeRotation(glm::quat());
		}
	}

	if (key == GLFW_KEY_0) {
		frameID = 0;
		for (uint i = 0; i < joints.size(); i++) {
			joints[i]->transform->SetRelativeRotation(glm::quat(motionFrames[frameID][i + 1] * TO_RADIANS));
		}
	}

	if (key == GLFW_KEY_EQUAL) {
		frameID++;
		frameID = frameID % totalFrames;
		for (uint i = 0; i < joints.size(); i++) {
			joints[i]->transform->SetRelativeRotation(glm::quat(motionFrames[frameID][i + 1] * TO_RADIANS));
		}
		animation->SetAnimationTime(frameID * frameTime);
	}
	if (key == GLFW_KEY_MINUS) {
		frameID--;
		frameID = frameID % totalFrames;
		for (uint i = 0; i < joints.size(); i++) {
			joints[i]->transform->SetRelativeRotation(glm::quat(motionFrames[frameID][i + 1] * TO_RADIANS));
		}
		animation->SetAnimationTime(frameID * frameTime);
	}
}
