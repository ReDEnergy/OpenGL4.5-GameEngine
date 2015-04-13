//#include <pch.h>
#include "SkinnedMesh.h"

#include <include/utils.h>
#include <include/assimp_utils.h>

#include <Component/Transform.h>
#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Manager/Manager.h>
#include <Manager/TextureManager.h>
#include <Manager/ShaderManager.h>

#include <Utils/GPU.h>

SkinnedMesh::SkinnedMesh() {
	meshType = MeshType::SKINNED;
	nrBones = 0;
}


SkinnedMesh::~SkinnedMesh() {
}

bool SkinnedMesh::LoadMesh(const std::string& fileName)
{
	Mesh::Clear();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	if (glPrimitive == GL_TRIANGLES) flags |= aiProcess_Triangulate;

	pScene = Importer.ReadFile(fileName.c_str(), flags);

	if (pScene) {
		assimp::CopyMatix(pScene->mRootNode->mTransformation, globalInvTransform);
		glm::inverse(globalInvTransform);
		return InitFromScene(pScene, fileName);
	}

	printf("Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
	return false;
}

bool SkinnedMesh::InitFromScene(const aiScene* pScene, const std::string& File)
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

	if (useMaterial && !InitMaterials(pScene, File))
		return false;

	buffers = UtilsGPU::UploadData<VertexBoneData>(positions, normals, texCoords, boneData, indices);
	return buffers->VAO != -1;
}

void SkinnedMesh::InitMesh(const aiMesh* paiMesh, unsigned int index)
{
	Mesh::InitMesh(paiMesh);

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
			uint VertexID = meshEntries[index]->baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			boneData[VertexID].AddBoneData(boneIndex, weight);
		}
	}
}

void SkinnedMesh::Update()
{
	float timeInSeconds = (float)glfwGetTime();
	BoneTransform(timeInSeconds);
}

void SkinnedMesh::BoneTransform(float timeInSeconds)
{
	glm::mat4 Identity(1.0f);

	float TicksPerSecond = (float)(pScene->mAnimations[0]->mTicksPerSecond != 0 ? pScene->mAnimations[0]->mTicksPerSecond : 25.0f);
	float TimeInTicks = timeInSeconds * TicksPerSecond;
	float animationTime = fmod(TimeInTicks, (float)pScene->mAnimations[0]->mDuration);

	ReadNodeHeirarchy(animationTime, pScene->mRootNode, Identity);

	for (uint i = 0; i < nrBones; i++) {
		boneTransform[i] = boneInfo[i].FinalTransformation;
	}
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	uint PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	uint NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	uint RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	uint NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	uint ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	uint NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}

void SkinnedMesh::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const glm::mat4 &ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = pScene->mAnimations[0];
	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	Transform boneT;
	assimp::CopyMatix(pNode->mTransformation, boneT.model);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, animationTime, pNodeAnim);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, animationTime, pNodeAnim);

		boneT.SetPosition(glm::vec3(Translation.x, Translation.y, Translation.z));
		boneT.SetRotationAndScale(glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z), glm::vec3(Scaling.x, Scaling.y, Scaling.z));
	}

	glm::mat4 GlobalTransformation = ParentTransform * boneT.model;

	if (skeletalBones.find(NodeName) != skeletalBones.end()) {
		uint BoneIndex = skeletalBones[NodeName];
		boneInfo[BoneIndex].FinalTransformation = globalInvTransform * GlobalTransformation * boneInfo[BoneIndex].BoneOffset;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(animationTime, pNode->mChildren[i], GlobalTransformation);
	}
}

uint SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


uint SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}


uint SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	assert(0);

	return 0;
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const string NodeName)
{
	for (uint i = 0; i < pAnimation->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == NodeName) {
			return pNodeAnim;
		}
	}

	return NULL;
}

void VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
	for (uint i = 0; i < SIZEOF_ARRAY(IDs); i++) {
		if (Weights[i] == 0.0) {
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	assert(0);
}

void SkinnedMesh::Render(const Shader *shader)
{
	glUniformMatrix4fv(shader->loc_bones[0], nrBones, GL_FALSE, glm::value_ptr(boneTransform[0]));
	Mesh::Render(shader);
}