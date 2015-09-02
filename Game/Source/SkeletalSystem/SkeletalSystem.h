#pragma once
#include "pch.h"

#include <unordered_map>

#include <Kinect/Kinect.h>

using namespace std;

class SkeletalJoint;
class SkeletalTracking;

class SkeletalSystem : public ObjectInput
{
	public:
		SkeletalSystem();
		~SkeletalSystem();

		void Init();
		void GetJoints(pugi::xml_node & node);
		SkeletalJoint *GetRoot() const;
		void Clear();

		void BindForComposition(GLenum DEPTH_TEXTURE_UNIT, GLenum CONTENT_TEXTURE_UNIT);

		void Render(Camera *camera) const;
		void Update(SkeletalTracking *tracking);

		void OnKeyPress(int key, int mods);

	private:
		void RecordSkeletalPhysicState(SkeletalTracking * tracking);

	public:
		unordered_map<string, SkeletalJoint*> joints;
		vector<SkeletalJoint*> kinectJointMapping;

	private:
		FrameBuffer *FBO;
		Shader *jointShader;
		SkeletalJoint *ROOT;
		bool recordKinectState;
		bool recordKinectRot;
		bool recordKinectHack;
};