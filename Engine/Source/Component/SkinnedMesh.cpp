#include "SkinnedMesh.h"

#include <iostream>

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

using namespace std;

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
	string file = (fileLocation + '/' + fileName).c_str();

	unsigned int flags =
		//aiProcess_CalcTangentSpace | // calculate tangents and bitangents if possible
		//aiProcess_JoinIdenticalVertices | // join identical vertices/ optimize indexing
		//aiProcess_ValidateDataStructure | // perform a full validation of the loader's output
		//aiProcess_ImproveCacheLocality | // improve the cache locality of the output vertices
		aiProcess_RemoveRedundantMaterials | // remove redundant materials
		aiProcess_FindDegenerates | // remove degenerated polygons from the import
		aiProcess_FindInvalidData | // detect invalid model data, such as invalid normal vectors
		//aiProcess_GenUVCoords | // convert spherical, cylindrical, box and planar mapping to proper UVs
		//aiProcess_TransformUVCoords | // preprocess UV transformations (scaling, translation ...)
		aiProcess_FindInstances | // search for instanced meshes and remove them by references to one master
		//aiProcess_LimitBoneWeights | // limit bone weights to 4 per vertex
		aiProcess_OptimizeMeshes | // join small meshes, if possible;
		//aiProcess_SplitByBoneCount | // split meshes with too many bones. Necessary for our (limited) hardware skinning shader
		aiProcess_GenSmoothNormals | // generate smooth normal vectors if not existing
		//aiProcess_SplitLargeMeshes | // split large, unrenderable meshes into submeshes
		aiProcess_Triangulate | // triangulate polygons with more than 3 edges
		aiProcess_FlipUVs |
		//aiProcess_ConvertToLeftHanded | // convert everything to D3D left handed space
		//aiProcess_SortByPType | // make 'clean' meshes which consist of a single typ of primitives
		0;

	pScene = Importer.ReadFile(file, flags);

	if (pScene) {
		assimp::CopyMatix(pScene->mRootNode->mTransformation, globalInvTransform);
		globalInvTransform = glm::inverse(globalInvTransform);
		return InitFromScene(pScene);
	}

	printf("Error parsing '%s': '%s'\n", file.c_str(), Importer.GetErrorString());
	return false;
}

bool SkinnedMesh::InitFromScene(const void* Scene)
{
	auto pScene = static_cast<const aiScene*>(Scene);

	Mesh::InitFromScene(pScene);
	boneData.resize(positions.size());

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < meshEntries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(paiMesh, i);
	}

	if (useMaterial && !InitMaterials(pScene))
		return false;

	// Skinned mesh information
	boneTransform.resize(nrBones);

	// Read animation data
	for (uint i = 0; i < pScene->mNumAnimations; i++) {
		animations[pScene->mAnimations[i]->mName.data] = pScene->mAnimations[i];
		if (pScene->mAnimations[i]->mTicksPerSecond == 0) {
			pScene->mAnimations[i]->mTicksPerSecond = pScene->mAnimations[i]->mDuration;
		}
	}

	if (loadState == MeshStatus::ERROR_MAX_INFLUENCE) {
		cout << "[ERROR]: MAX_INFLUENCE" << endl;
		//return false;
	}

	*buffers = UtilsGPU::UploadData<VertexBoneData>(positions, normals, texCoords, boneData, indices);
	return buffers->VAO != 0;
}

void SkinnedMesh::InitMesh(const aiMesh* paiMesh, uint index)
{
	Mesh::InitMesh(paiMesh);

	uint *bonesPerVertex = new uint[boneData.size()]();

	glm::mat4 boneOffset;

	for (unsigned int i = 0; i < paiMesh->mNumBones; i++) {
		unsigned int boneIndex = 0;
		string boneName(paiMesh->mBones[i]->mName.data);

		if (skeletalJoints.find(boneName) == skeletalJoints.end()) {
			// Allocate an index for a new joint
			boneIndex = nrBones;

			// TODO - keep boneOffset and TPoseOffset in the SkinnedMesh class - must be used by all instances
			auto SKJ = new SkeletalJoint(boneName.c_str(), boneIndex);
			assimp::CopyMatix(paiMesh->mBones[i]->mOffsetMatrix, boneOffset);
			SKJ->SetBoneOffset(boneOffset);

			skeletalJoints[boneName] = SKJ;
			nrBones++;
		}
		else {
			boneIndex = skeletalJoints[boneName]->GetJointID();
		}

		for (unsigned int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++)
		{
			uint VertexID = meshEntries[index].baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[j].mWeight;

			int index = bonesPerVertex[VertexID];
			if (index == NUM_BONES_PER_VEREX) {
				loadState = MeshStatus::ERROR_MAX_INFLUENCE;
			}
			else {
				boneData[VertexID].SetBoneData(index, boneIndex, weight);
				bonesPerVertex[VertexID]++;
			}
		}
	}

	for (auto &joint : skeletalJoints) {
		auto node = pScene->mRootNode->FindNode(joint.first.c_str());
		auto parent = skeletalJoints.find(node->mParent->mName.C_Str());
		if (parent != skeletalJoints.end()) {
			parent->second->AddChild(joint.second);
		}
		//assimp::CopyMatix(node->mTransformation, joint.second->TPoseOffset);
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

aiAnimation * SkinnedMesh::GetAnimationByID(uint animationID) const
{
	unsigned int k = 0;
	for (auto &animation : animations) {
		if (k == animationID)
			return animation.second;
		k++;
	}
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

std::unordered_map<std::string, SkeletalJoint*> SkinnedMesh::GetSkeletonCopy(SkeletalJoint *&ROOT) const
{
	std::unordered_map<std::string, SkeletalJoint*> joints;

	// First copy skeleton joints
	for (auto &joint : skeletalJoints) {
		joints[joint.first] = new SkeletalJoint(*joint.second);
	}
	ROOT = joints[rootJoint->GetName()];

	// Rebuild hierarchy using original data
	for (auto &joint : skeletalJoints) {
		auto parent = joint.second->GetParent();
		if (parent) {
			joints[joint.first]->SetParent(joints[parent->GetName()]);
		}
	}

	return joints;
}

void VertexBoneData::SetBoneData(uint index, uint BoneID, float Weight)
{
	IDs[index] = BoneID;
	Weights[index] = Weight;
}
