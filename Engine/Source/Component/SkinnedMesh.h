#pragma once
#include <vector>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/gl.h>
#include <include/glm.h>
#include <include/utils.h>

#include <Component/Mesh.h>

#include <unordered_map>

using namespace std;

#define NUM_BONES_PER_VEREX 4

class Shader;
class SkeletalJoint;
class Transform;

struct VertexBoneData
{
	unsigned int IDs[NUM_BONES_PER_VEREX];
	float Weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		ZERO_MEM(IDs);
		ZERO_MEM(Weights);
	}

	uint AddBoneData(uint BoneID, float Weight);
};

class DLLExport SkinnedMesh : public Mesh
{
	public:
		enum class PLAYBACK {
			PLAY,
			PAUSE,
			TOGGLE,
		};

	public:
		SkinnedMesh(const char* meshID = NULL);
		~SkinnedMesh();

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		void Render(const Shader* shader);
		void RenderDebug(const Shader * shader) const;

		void Update();
		void SetAnimationState(const char *animationState);
		bool ToggleAnimationPlayback(PLAYBACK state = PLAYBACK::TOGGLE);
		void ScaleAnimationTime(const string &animation, float timeScale);
		void DrawControlSkeleton();

	private:
		bool InitFromScene(const aiScene* pScene);
		void InitMesh(const aiMesh* paiMesh, uint index);
		void InitControlSkeleton();

		void UpdateGPUData();
		void UpdateAnimation();
		void UpdateAnimationNodesMapping();
		void UpdateJointTransform(SkeletalJoint * joint, float animationTime);
		void ReadNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4 &ParentTransform);

		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);

		uint FindPositionKeyID(float animationTime, const aiNodeAnim* pNodeAnim);
		uint FindRotationKeyID(float animationTime, const aiNodeAnim* pNodeAnim);
		uint FindScalingKeyID(float animationTime, const aiNodeAnim* pNodeAnim);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);

	private:
		unordered_map<string, SkeletalJoint*> skeletalBones;
		vector<VertexBoneData> boneData;
		vector<glm::mat4> boneTransform;
		unsigned short nrBones;

		// TODO - Support for multiple skinned meshes
		// Right now only a single ROOT joint is considered
		SkeletalJoint *rootJoint;
		glm::mat4 rootTransform;
		glm::mat4 globalInvTransform;

		Assimp::Importer Importer;
		const aiScene* pScene;

		unordered_map<string, aiAnimation*> animations;
		aiAnimation *animationState;
		bool playbackState;
		bool controlState;
};