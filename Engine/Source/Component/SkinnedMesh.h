#pragma once
#include <vector>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/gl.h>
#include <include/glm.h>
#include <include/util.h>

#include <Component/Mesh.h>

#include <unordered_map>

using namespace std;

#define NUM_BONES_PER_VEREX 4


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

class SkinnedMesh : public Mesh
{
	public:
		SkinnedMesh();
		~SkinnedMesh();

		bool LoadMesh(const std::string& fileName);
		void Render();

	private:
		virtual bool InitFromScene(const aiScene* pScene, const std::string& File);
		virtual void InitMesh(const aiMesh* paiMesh, uint index);

	private:
		unordered_map<string, uint> skeletalBones;
		vector<VertexBoneData> bones;
		vector<BoneInfo> boneInfo;
		unsigned short nrBones;
};