#include "pch.h"

#include "Skeleton.h"

Skeleton::Skeleton(const char* meshID)
{
	nrBones = 0;
}


Skeleton::~Skeleton() {
}

bool Skeleton::LoadMesh(const std::string& fileLocation, const std::string& fileName)
{
	//Mesh::Clear();
	//this->fileLocation = fileLocation;
	string file = (fileLocation + '\\' + fileName).c_str();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	//if (glPrimitive == GL_TRIANGLES) flags |= aiProcess_Triangulate;

	pScene = Importer.ReadFile(file, flags);

	if (pScene) {
		assimp::CopyMatix(pScene->mRootNode->mTransformation, globalInvTransform);
		glm::inverse(globalInvTransform);
		return InitFromScene(pScene);
	}

	printf("Error parsing '%s': '%s'\n", file, Importer.GetErrorString());
	return false;
}

bool Skeleton::InitFromScene(const aiScene* pScene)
{

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(paiMesh, i);
	}

	return 0;
}

void Skeleton::InitMesh(const aiMesh* paiMesh, unsigned int index)
{
	for (unsigned int i = 0; i < paiMesh->mNumBones; i++) {
		unsigned int boneIndex = 0;
		string boneName(paiMesh->mBones[i]->mName.data);

		if (skeletalBones.find(boneName) == skeletalBones.end()) {
			// Allocate an index for a new bone
			boneIndex = nrBones;
			BoneInfo bi;
			assimp::CopyMatix(paiMesh->mBones[i]->mOffsetMatrix, bi.BoneOffset);
			boneInfo.push_back(bi);
			skeletalBones[boneName] = boneIndex;
			nrBones++;
		}
		else {
			boneIndex = skeletalBones[boneName];
		}

		for (unsigned int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
			//uint VertexID = meshEntries[index]->baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			//boneData[VertexID].AddBoneData(boneIndex, weight);
		}
	}
}
