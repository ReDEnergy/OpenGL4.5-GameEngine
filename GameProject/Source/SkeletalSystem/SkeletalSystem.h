#pragma once
#include <vector>
#include <unordered_map>

#include <Component/ObjectInput.h>
#include <Event/EventListener.h>

class GameObject;
class GSkeletalJoint;
class KinectSkeletalTracking;
class KinectAvatar;
class AnimationSystem;

class SkeletalSystem :
	public ObjectInput,
	public EventListener
{
	public:
		SkeletalSystem();
		~SkeletalSystem();

		void Init();

		const std::unordered_map<std::string, GSkeletalJoint*>& GetSkeletalJoints() const;
		const std::vector<GSkeletalJoint*>& GetKinectJointMapping() const;

		void Update();

	private:
		void Clear();
		void GetJoints(pugi::xml_node & node);
#ifdef KINECT_SENSOR
		void RecordSkeletalPhysicState(KinectSkeletalTracking * tracking);
#endif
		void OnEvent(const std::string& eventID, void *data);
		void OnKeyPress(int key, int mods);

	private:
		std::unordered_map<std::string, GSkeletalJoint*> joints;
		std::vector<GSkeletalJoint*> kinectJointMapping;

		KinectAvatar *avatar;
		GSkeletalJoint *ROOT;
		GameObject *skeletonControl;

		bool recordKinectState;
		bool readKinectPos;
		bool readKinectRot;
};