#pragma once
#include <vector>
#include <unordered_map>

#include <Kinect/Kinect.h>

class GSkeletalJoint;
class SkeletalTracking;
class KinectAvatar;
class GameObject;

class SkeletalSystem :
	public ObjectInput,
	public EventListener
{
	public:
		SkeletalSystem();
		~SkeletalSystem();

		void Init();
		void GetJoints(pugi::xml_node & node);
		void Clear();

		const std::unordered_map<std::string, GSkeletalJoint*>& GetJoints() const;

		void BindForComposition(GLenum DEPTH_TEXTURE_UNIT, GLenum CONTENT_TEXTURE_UNIT);

		void Update(SkeletalTracking *tracking);

		void OnKeyPress(int key, int mods);

	private:
		void RecordSkeletalPhysicState(SkeletalTracking * tracking);

	protected:
		void OnEvent(const std::string& eventID, void *data);

	public:
		std::unordered_map<std::string, GSkeletalJoint*> joints;
		std::vector<GSkeletalJoint*> kinectJointMapping;

	private:
		KinectAvatar *avatar;

		FrameBuffer *FBO;
		Shader *jointShader;
		GSkeletalJoint *ROOT;
		GameObject *skeletonControl;

		bool recordKinectState;
		bool readKinectPos;
		bool readKinectRot;
};