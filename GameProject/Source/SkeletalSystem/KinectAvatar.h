#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>

#include <include/glm.h>
#include <Core/GameObject.h>
#include <Component/ObjectInput.h>

class GSkeletalJoint;
class SkeletalJoint;

class KinectAvatar
	: public GameObject,
	public ObjectInput
{
	public:
		KinectAvatar(const char* meshName);
		~KinectAvatar();

		void ReadBVH();
		void ReadBVHJoint(std::ifstream &F, GameObject* parent);

		void Update();
		void Render(const Shader *shader) const;

	private:
		void OnKeyPress(int key, int mods);

	private:
		float avatarScale;
		float motionScale;
		bool playMotion;
		bool scaleSkeleton;
		unsigned int frameID;
		unsigned int totalFrames;
		float frameTime;
		float lastMotionUpdate;
		std::vector<GSkeletalJoint*> joints;
		std::vector<glm::vec3> *motionFrames;
		std::vector<glm::mat4> boneTransform;
};