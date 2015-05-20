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

inline uint GetNextAnimationKey(uint currentKey, uint maxKeys)
{
	return (currentKey + 1) % maxKeys;
}

SkinnedMesh::SkinnedMesh(const char* meshID)
	: Mesh(meshID)
{
	meshType = MeshType::SKINNED;
	nrBones = 0;
}


SkinnedMesh::~SkinnedMesh() {
}

bool SkinnedMesh::LoadMesh(const std::string& fileLocation, const std::string& fileName)
{
	Mesh::Clear();
	this->fileLocation = fileLocation;
	string file = (fileLocation + '\\' + fileName).c_str();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	if (glPrimitive == GL_TRIANGLES) flags |= aiProcess_Triangulate;

	pScene = Importer.ReadFile(file, flags);

	if (pScene) {
		assimp::CopyMatix(pScene->mRootNode->mTransformation, globalInvTransform);
		glm::inverse(globalInvTransform);
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

	animationState = pScene->mAnimations[0];

	if (useMaterial && !InitMaterials(pScene))
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
	UpdateAnimation(timeInSeconds);
}

void SkinnedMesh::SetAnimationState(char *animationState)
{
	if (animations.find(animationState) != animations.end())
		this->animationState = animations[animationState];
	else {
		cout << "Animation State: " << animationState << " not found" << endl;
	}
}

void SkinnedMesh::ScaleAnimationTime(const string &animation, float timeScale)
{
	animations[animation]->mTicksPerSecond = timeScale * animations[animation]->mDuration;
}

void SkinnedMesh::UpdateAnimation(float timeInSeconds)
{
	float TimeInTicks = timeInSeconds * (float)animationState->mTicksPerSecond;
	float animationTime = fmod(TimeInTicks, (float)animationState->mDuration);

	ReadNodeHeirarchy(animationTime, pScene->mRootNode, glm::mat4(1.0f));

	for (uint i = 0; i < nrBones; i++) {
		boneTransform[i] = boneInfo[i].FinalTransformation;
	}
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	aiVectorKey *positionKeys = pNodeAnim->mPositionKeys;

	uint posKeyID = FindPositionKeyID(animationTime, pNodeAnim);
	uint nextPosKeyID = GetNextAnimationKey(posKeyID, pNodeAnim->mNumPositionKeys);

	float keyDeltaTime = 0;
	if (nextPosKeyID == 0) {
		keyDeltaTime = float(animationState->mDuration - positionKeys[posKeyID].mTime + positionKeys[0].mTime);
	}
	else {
		keyDeltaTime = float(positionKeys[nextPosKeyID].mTime - positionKeys[posKeyID].mTime);
	}

	float factor = (animationTime - (float)positionKeys[posKeyID].mTime) / keyDeltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = positionKeys[posKeyID].mValue;
	const aiVector3D& end = positionKeys[nextPosKeyID].mValue;
	aiVector3D delta = end - start;
	out = start + factor * delta;
}


void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	aiQuatKey *rotationKeys = pNodeAnim->mRotationKeys;

	uint rotKeyID = FindRotationKeyID(animationTime, pNodeAnim);
	uint nextRotKeyID = GetNextAnimationKey(rotKeyID, pNodeAnim->mNumRotationKeys);

	float keyDeltaTime = 0;
	if (nextRotKeyID == 0) {
		keyDeltaTime = float(animationState->mDuration - rotationKeys[rotKeyID].mTime + rotationKeys[0].mTime);
	}
	else {
		keyDeltaTime = float(rotationKeys[nextRotKeyID].mTime - rotationKeys[rotKeyID].mTime);
	}

	float factor = (animationTime - (float)rotationKeys[rotKeyID].mTime) / keyDeltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);

	const aiQuaternion& startRotationQ = rotationKeys[rotKeyID].mValue;
	const aiQuaternion& endRotationQ = rotationKeys[nextRotKeyID].mValue;
	aiQuaternion::Interpolate(out, startRotationQ, endRotationQ, factor);
	out = out.Normalize();
}


void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& out, float animationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	aiVectorKey *scalingKeys = pNodeAnim->mScalingKeys;

	uint scalingKeyID = FindScalingKeyID(animationTime, pNodeAnim);
	uint nextScalingKeyID = GetNextAnimationKey(scalingKeyID, pNodeAnim->mNumScalingKeys);

	float keyDeltaTime = 0;
	if (nextScalingKeyID == 0) {
		keyDeltaTime = float(animationState->mDuration - scalingKeys[scalingKeyID].mTime + scalingKeys[0].mTime);
	}
	else {
		keyDeltaTime = float(scalingKeys[nextScalingKeyID].mTime - scalingKeys[scalingKeyID].mTime);
	}

	float factor = (animationTime - (float)scalingKeys[scalingKeyID].mTime) / keyDeltaTime;
	assert(factor >= 0.0f && factor <= 1.0f);

	const aiVector3D& start = scalingKeys[scalingKeyID].mValue;
	const aiVector3D& end = scalingKeys[nextScalingKeyID].mValue;
	aiVector3D delta = end - start;
	out = start + factor * delta;
}

void SkinnedMesh::ReadNodeHeirarchy(float animationTime, const aiNode* pNode, const glm::mat4 &ParentTransform)
{
	string NodeName(pNode->mName.data);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(animationState, NodeName);

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

// Seems that assimp sets minimum number of keys = 2
// Safe mode - we return the maximum number of keys - 2

uint SkinnedMesh::FindPositionKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (animationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumPositionKeys - 1;
}

uint SkinnedMesh::FindRotationKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (animationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumRotationKeys - 1;
}


uint SkinnedMesh::FindScalingKeyID(float animationTime, const aiNodeAnim* pNodeAnim)
{
	for (uint i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (animationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumScalingKeys - 1;
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