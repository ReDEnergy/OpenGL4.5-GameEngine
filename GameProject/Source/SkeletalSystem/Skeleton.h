#pragma once
#include <PCH/pch.h>

#include <vector>
#include <unordered_map>

using namespace std;

#define NUM_BONES_PER_VEREX 4

class Shader;

class Skeleton
{
	public:
		Skeleton(const char* meshID = NULL);
		~Skeleton();

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);

	private:
		bool InitFromScene(const aiScene* pScene);
		void InitMesh(const aiMesh * paiMesh, unsigned int index);

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