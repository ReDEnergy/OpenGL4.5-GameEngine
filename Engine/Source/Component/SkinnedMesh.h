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

struct BoneInfo
{
	glm::mat4 BoneOffset;
	glm::mat4 FinalTransformation;

	BoneInfo()
	{
		BoneOffset = glm::mat4(0);
		FinalTransformation = glm::mat4(0);
	}
};

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

	void AddBoneData(uint BoneID, float Weight);
};

class DLLExport SkinnedMesh : public Mesh
{
	public:
		SkinnedMesh(const char* meshID = NULL);
		~SkinnedMesh();

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		void Render(const Shader* shader);
		void Update();
		void SetAnimationState(char *animationState);

	private:
		bool InitFromScene(const aiScene* pScene);
		void InitMesh(const aiMesh* paiMesh, uint index);

		void UpdateAnimation(float timeInSeconds);
		void ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const glm::mat4 &ParentTransform);

		void CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim);
		void CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim);

		uint FindPositionKeyID(float animationTime, const aiNodeAnim* pNodeAnim);
		uint FindRotationKeyID(float animationTime, const aiNodeAnim* pNodeAnim);
		uint FindScalingKeyID(float animationTime, const aiNodeAnim* pNodeAnim);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const string NodeName);

	private:
		unordered_map<string, uint> skeletalBones;
		vector<VertexBoneData> boneData;
		vector<BoneInfo> boneInfo;
		vector<glm::mat4> boneTransform;
		unsigned short nrBones;
		glm::mat4 globalInvTransform;

		Assimp::Importer Importer;
		const aiScene* pScene;

		unordered_map<string, aiAnimation*> animations;
		aiAnimation *animationState;

};