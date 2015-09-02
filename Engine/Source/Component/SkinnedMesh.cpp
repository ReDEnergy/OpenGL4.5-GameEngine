//#include <pch.h>
#include "SkinnedMesh.h"

#include <include/utils.h>
#include <include/gl_utils.h>
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

inline uint GetNextAnimationKey(uint currentKey, uint maxKeys)
{
	return (currentKey + 1) % maxKeys;
}

SkinnedMesh::SkinnedMesh(const char* meshID)
	: Mesh(meshID)
{
	meshType = MeshType::SKINNED;
	animationState = nullptr;
	playbackState = true;
	controlState = true;
	rootJoint = nullptr;
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

	if (pScene->mNumAnimations) {
		SetAnimationState(pScene->mAnimations[0]->mName.C_Str());
	}

	if (useMaterial && !InitMaterials(pScene))
		return false;

	if (loadState == LOG_MESH::ERROR_MAX_INFLUENCE) {
		cout << "ERROR: MAX_INFLUENCE" << endl;
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

		if (skeletalBones.find(boneName) == skeletalBones.end()) {
			// Allocate an index for a new bone
			boneIndex = nrBones;
			auto SKJ = new SkeletalJoint(boneName.c_str(), boneIndex);
			SKJ->SetName(boneName.c_str());
			assimp::CopyMatix(paiMesh->mBones[i]->mOffsetMatrix, SKJ->boneOffset);
			skeletalBones[boneName] = SKJ;
			nrBones++;
		}
		else {
			boneIndex = skeletalBones[boneName]->GetJointID();
		}

		for (unsigned int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = meshEntries[index]->baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			if (boneData[VertexID].AddBoneData(boneIndex, weight)) {
				loadState = LOG_MESH::ERROR_MAX_INFLUENCE;
			}
		}
	}

	InitControlSkeleton();
}

void SkinnedMesh::InitControlSkeleton()
{
	for (auto &bone : skeletalBones) {
		auto node = pScene->mRootNode->FindNode(bone.first.c_str());
		auto parent = skeletalBones.find(node->mParent->mName.C_Str());
		if (parent != skeletalBones.end()) {
			parent->second->AddChild(bone.second);
		}
	}

	// Get Root Joint
	GameObject *joint = skeletalBones.begin()->second;
	while (joint->GetParent()) {
		joint = joint->GetParent();
	}
	rootJoint = (SkeletalJoint*)joint;

	auto node = pScene->mRootNode->FindNode(rootJoint->GetName());
	assimp::CopyMatix(node->mParent->mTransformation, rootTransform);
}

void SkinnedMesh::DrawControlSkeleton()
{
	auto mesh = Manager::GetResource()->GetMesh("box");

	for (auto &bone : skeletalBones) {
		bone.second->SetMesh(mesh);
	}

	// Add Skeleton to Scene
	Manager::Scene->AddObject((GameObject*)rootJoint);
}

void SkinnedMesh::Update()
{
	if (!(animationState && playbackState)) {
		if (controlState) {
			rootJoint->globalTransform = rootTransform;
			rootJoint->UpdateSkeleton(globalInvTransform);
			UpdateGPUData();
		}
		return;
	}

	UpdateAnimation();
	UpdateGPUData();
}

void SkinnedMesh::SetAnimationState(const char *animationState)
{
	if (animationState == nullptr) {
		this->animationState = nullptr;
		return;
	}

	if (animations.find(animationState) != animations.end()) {
		this->animationState = animations[animationState];
		UpdateAnimationNodesMapping();
	}
	else {
		cout << "Animation State: " << animationState << " not found" << endl;
	}
}

bool SkinnedMesh::ToggleAnimationPlayback(PLAYBACK state)
{
	switch (state)
	{
	case SkinnedMesh::PLAYBACK::PLAY:
		playbackState = true;
		break;
	case SkinnedMesh::PLAYBACK::PAUSE:
		playbackState = false;
		break;
	default:	// SkinnedMesh::PLAYBACK::TOGGLE
		playbackState = !playbackState;
		break;
	}
	return playbackState;
}

void SkinnedMesh::ScaleAnimationTime(const string &animation, float timeScale)
{
	animations[animation]->mTicksPerSecond = timeScale * animations[animation]->mDuration;
}

void SkinnedMesh::UpdateGPUData()
{
	for (auto &bone : skeletalBones) {
		auto id = bone.second->GetJointID();
		boneTransform[id] = bone.second->finalTransformation;
	}
}

void SkinnedMesh::UpdateAnimation()
{
	float timeInSeconds = (float)glfwGetTime();
	float TimeInTicks = timeInSeconds * (float)animationState->mTicksPerSecond;
	float animationTime = fmod(TimeInTicks, (float)animationState->mDuration);

	//ReadNodeHierarchy(animationTime, pScene->mRootNode, glm::mat4(1.0f));
	rootJoint->globalTransform = rootTransform;
	UpdateJointTransform(rootJoint, animationTime);
}

void SkinnedMesh::UpdateAnimationNodesMapping()
{
	for (auto &bone : skeletalBones) {
		bone.second->pAnimationNode = FindNodeAnim(animationState, bone.first);
	}
}

void SkinnedMesh::UpdateJointTransform(SkeletalJoint* joint, float animationTime)
{
	// Interpolate scaling and generate scaling transformation matrix
	aiVector3D Scaling;
	CalcInterpolatedScaling(Scaling, animationTime, joint->pAnimationNode);

	// Interpolate rotation and generate rotation transformation matrix
	aiQuaternion RotationQ;
	CalcInterpolatedRotation(RotationQ, animationTime, joint->pAnimationNode);

	// Interpolate translation and generate translation transformation matrix
	aiVector3D Translation;
	CalcInterpolatedPosition(Translation, animationTime, joint->pAnimationNode);

	// Update transform
	joint->transform->SetWorldPosition(glm::vec3(Translation.x, Translation.y, Translation.z));
	joint->transform->SetWorldRotationAndScale(
		glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z),
		glm::vec3(Scaling.x, Scaling.y, Scaling.z));

	joint->UpdateTransform(globalInvTransform);

	for (auto J : joint->GetChildren()) {
		UpdateJointTransform((SkeletalJoint*)J, animationTime);
	}
}

void SkinnedMesh::ReadNodeHierarchy(float animationTime, const aiNode* pNode, const glm::mat4 &ParentTransform)
{
	glm::mat4 GlobalTransformation = ParentTransform;
	string NodeName(pNode->mName.data);

	auto pNodeAnim = FindNodeAnim(animationState, NodeName);
	if (pNodeAnim)
	{
		if (skeletalBones.find(NodeName) != skeletalBones.end()) 
		{
			// Bone transform
			// uint BoneIndex = skeletalBones[NodeName]->GetJointID();
			auto bone = skeletalBones[NodeName];
			auto boneT = bone->transform;

			if (playbackState)
			{
				// Interpolate scaling and generate scaling transformation matrix
				aiVector3D Scaling;
				CalcInterpolatedScaling(Scaling, animationTime, pNodeAnim);

				// Interpolate rotation and generate rotation transformation matrix
				aiQuaternion RotationQ;
				CalcInterpolatedRotation(RotationQ, animationTime, pNodeAnim);

				// Interpolate translation and generate translation transformation matrix
				aiVector3D Translation;
				CalcInterpolatedPosition(Translation, animationTime, pNodeAnim);

				// Update transform
				boneT->SetWorldPosition(glm::vec3(Translation.x, Translation.y, Translation.z));
				boneT->SetWorldRotationAndScale(
					glm::quat(RotationQ.w, RotationQ.x, RotationQ.y, RotationQ.z),
					glm::vec3(Scaling.x, Scaling.y, Scaling.z));
			}

			GlobalTransformation *= boneT->GetModel();
			bone->globalTransform = GlobalTransformation;
			bone->finalTransformation = globalInvTransform * GlobalTransformation * bone->boneOffset;
		}
	}
	else {
		glm::mat4 temp;
		assimp::CopyMatix(pNode->mTransformation, temp);
		GlobalTransformation *= temp;
		if (pNode->mNumChildren) {
			cout << pNode->mNumChildren << endl;
		}
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHierarchy(animationTime, pNode->mChildren[i], GlobalTransformation);
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

void SkinnedMesh::Render(const Shader *shader)
{
	glUniform1f(shader->loc_animated, animationState ? true : false);
	glUniformMatrix4fv(shader->loc_bones[0], nrBones, GL_FALSE, glm::value_ptr(boneTransform[0]));
	Mesh::Render(shader);
}

void SkinnedMesh::RenderDebug(const Shader * shader) const
{
	glDepthMask(GL_FALSE);
	GL_Utils::SetColorUnit(shader->loc_debug_color, 1, 1, 0);
	auto obj = Manager::Resource->GetPropObject("box");
	for (auto &bone: skeletalBones) {
		glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(bone.second->transform->GetModel()));
		obj->mesh->Render(shader);
	}
	glDepthMask(GL_TRUE);
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