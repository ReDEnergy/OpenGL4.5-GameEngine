#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/glm.h>
#include <include/utils.h>

#include <Component/Mesh.h>

#include <unordered_map>

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
		memset(IDs, 0, sizeof(IDs));
		memset(Weights, 0, sizeof(IDs));
	}

	void SetBoneData(uint index, uint BoneID, float Weight);
};


// TODO
// Move the aniamtion logic from the class to an animation class - that's rated to a game object
// Problem: Instantiating the same mesh with different animations is not possible right know
// Animation logic is liked to the SkinnedMesh - It shouldn't

class DLLExport SkinnedMesh : public Mesh
{
	public:
		SkinnedMesh(const char* meshID = NULL);
		~SkinnedMesh();

		bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		void ScaleAnimationTime(const std::string &animation, float timeScale);
		
		// Return a new copy of the Skeleton
		std::unordered_map<std::string, SkeletalJoint*> GetSkeletonCopy(SkeletalJoint* &ROOT) const;

		#ifdef ENGINE_DLL_EXPORTS
		aiAnimation* GetAnimation(const char *name) const;
		aiAnimation* GetAnimationByID(uint animationID) const;
		#endif

		uint GetNumberOfBones() const;
		glm::mat4 GetRootTransform() const;
		glm::mat4 GetGlobalInverse() const;

	private:
		#ifdef ENGINE_DLL_EXPORTS
		bool InitFromScene(const aiScene* pScene) override;
		void InitMesh(const aiMesh* paiMesh, uint index);
		#endif

	private:
		std::unordered_map<std::string, SkeletalJoint*> skeletalJoints;
		std::vector<VertexBoneData> boneData;
		std::vector<glm::mat4> boneTransform;
		unsigned short nrBones;

		// TODO - Support for multiple skinned meshes
		// Right now only a single ROOT joint is considered
		SkeletalJoint *rootJoint;
		glm::mat4 rootTransform;
		glm::mat4 globalInvTransform;

		#ifdef ENGINE_DLL_EXPORTS
		Assimp::Importer Importer;
		const aiScene* pScene;

		std::unordered_map<std::string, aiAnimation*> animations;
		aiAnimation *animationState;
		#endif
};