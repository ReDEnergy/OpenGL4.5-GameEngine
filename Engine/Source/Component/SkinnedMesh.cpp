#include "SkinnedMesh.h"

#include <iostream>
using namespace std;

#include <include/utils.h>
#include <include/gl_utils.h>
#include <include/glm_utils.h>
#include <include/assimp_utils.h>

#include <Component/Transform/Transform.h>
#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Core/Camera/Camera.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/TextureManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

#include <SkeletalSystem/SkeletalJoint.h>

#include <Utils/GPU.h>

SkinnedMesh::SkinnedMesh(const char* meshID)
	: Mesh(meshID)
{
	meshType = MeshType::SKINNED;
	animationState = nullptr;
	rootJoint = nullptr;
	nrBones = 0;
}


SkinnedMesh::~SkinnedMesh() {
}

bool SkinnedMesh::LoadMesh(const std::string& fileLocation, const std::string& fileName)
{
	Mesh::ClearData();
	this->fileLocation = fileLocation;
	string file = (fileLocation + '\\' + fileName).c_str();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	if (glPrimitive == GL_TRIANGLES) flags |= aiProcess_Triangulate;

	pScene = Importer.ReadFile(file, flags);

	if (pScene) {
		assimp::CopyMatix(pScene->mRootNode->mTransformation, globalInvTransform);
		globalInvTransform = glm::inverse(globalInvTransform);
		return InitFromScene(pScene);
	}

	printf("Error parsing '%s': '%s'\n", file, Importer.GetErrorString());
	return false;
}

bool SkinnedMesh::InitFromScene(const aiScene* pScene)
{
	meshEntries.resize(pScene->mNumMeshes);
	materials.resize(pScene->mNumMaterials);

	uint nrVertices = 0;
	uint nrIndices = 0;

	// Count the number of vertices and indices
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		meshEntries[i] = new MeshEntry();
		meshEntries[i]->materialIndex = pScene->mMeshes[i]->mMaterialIndex;
		meshEntries[i]->nrIndices = pScene->mMeshes[i]->mNumFaces * (glPrimitive == GL_TRIANGLES ? 3 : 4);
		meshEntries[i]->baseVertex = nrVertices;
		meshEntries[i]->baseIndex = nrIndices;

		nrVertices += pScene->mMeshes[i]->mNumVertices;
		nrIndices += meshEntries[i]->nrIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	positions.reserve(nrVertices);
	normals.reserve(nrVertices);
	texCoords.reserve(nrVertices);
	indices.reserve(nrIndices);
	boneData.resize(nrVertices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < meshEntries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(paiMesh, i);
	}

	boneTransform.resize(nrBones);

	// Read animation data
	for (uint i = 0; i < pScene->mNumAnimations; i++) {
		animations[pScene->mAnimations[i]->mName.data] = pScene->mAnimations[i];
		if (pScene->mAnimations[i]->mTicksPerSecond == 0) {
			pScene->mAnimations[i]->mTicksPerSecond = pScene->mAnimations[i]->mDuration;
		}
	}

	if (useMaterial && !InitMaterials(pScene))
		return false;

	if (loadState == LOG_MESH::ERROR_MAX_INFLUENCE) {
		cout << "[ERROR]: MAX_INFLUENCE" << endl;
		return false;
	}

	buffers = UtilsGPU::UploadData<VertexBoneData>(positions, normals, texCoords, boneData, indices);
	return buffers->VAO != -1;
}

void SkinnedMesh::InitMesh(const aiMesh* paiMesh, uint index)
{
	Mesh::InitMesh(paiMesh);

	for (unsigned int i = 0; i < paiMesh->mNumBones; i++) {
		unsigned int boneIndex = 0;
		string boneName(paiMesh->mBones[i]->mName.data);

		if (skeletalJoints.find(boneName) == skeletalJoints.end()) {
			// Allocate an index for a new joint
			boneIndex = nrBones;

			// TODO - keep boneOffset and TPoseOffset in the SkinnedMesh class - must be used by all instances
			auto SKJ = new SkeletalJoint(boneName.c_str(), boneIndex);
			SKJ->SetName(boneName.c_str());

			assimp::CopyMatix(paiMesh->mBones[i]->mOffsetMatrix, SKJ->boneOffset);

			skeletalJoints[boneName] = SKJ;
			nrBones++;
		}
		else {
			boneIndex = skeletalJoints[boneName]->GetJointID();
		}

		for (unsigned int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++)
		{
			uint VertexID = meshEntries[index]->baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			if (boneData[VertexID].AddBoneData(boneIndex, weight)) {
				loadState = LOG_MESH::ERROR_MAX_INFLUENCE;
			}
		}
	}

	for (auto &joint : skeletalJoints) {
		auto node = pScene->mRootNode->FindNode(joint.first.c_str());
		auto parent = skeletalJoints.find(node->mParent->mName.C_Str());
		if (parent != skeletalJoints.end()) {
			parent->second->AddChild(joint.second);
		}
		assimp::CopyMatix(node->mTransformation, joint.second->TPoseOffset);
	}

	// Get Root Joint
	GameObject *joint = skeletalJoints.begin()->second;
	while (joint->GetParent()) {
		joint = joint->GetParent();
	}
	rootJoint = (SkeletalJoint*)joint;

	auto node = pScene->mRootNode->FindNode(rootJoint->GetName());
	assimp::CopyMatix(node->mParent->mTransformation, rootTransform);
}

SkeletalJoint* SkinnedMesh::GetSkeletonInfo(unordered_map<string, SkeletalJoint*> &joints) const
{
	SkeletalJoint * root = nullptr;

	// First copy skeleton joints
	for (auto &joint : skeletalJoints) {
		joints[joint.first] = new SkeletalJoint(*joint.second);
	}

	// Rebuild hierarchy using original data
	for (auto &joint : skeletalJoints) {
		auto parent = joint.second->GetParent();
		if (parent) {
			joints[joint.first]->SetParent(joints[parent->GetName()]);
		}
		else {
			root = joints[joint.first];
		}
	}
	return root;
}

aiAnimation * SkinnedMesh::GetAnimation(const char * name) const
{
	if (!name)
		return nullptr;

	if (animations.find(name) != animations.end()) {
		return animations.at(name);
	}
	cout << "[ANIMATION] [" << name << "] was not found" << endl;
	return nullptr;
}

uint SkinnedMesh::GetNumberOfBones() const
{
	return nrBones;
}

glm::mat4 SkinnedMesh::GetRootTransform() const
{
	return rootTransform;
}

glm::mat4 SkinnedMesh::GetGlobalInverse() const
{
	return globalInvTransform;
}

void SkinnedMesh::ScaleAnimationTime(const string &animation, float timeScale)
{
	animations[animation]->mTicksPerSecond = timeScale * animations[animation]->mDuration;
}

uint VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (uint i = 0; i < SIZEOF_ARRAY(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return 0;
		}
	}
	return 1;
}