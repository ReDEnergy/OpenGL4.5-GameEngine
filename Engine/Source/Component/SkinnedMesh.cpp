//#include <pch.h>
#include "SkinnedMesh.h"

#include <include/utils.h>
#include <include/assimp_utils.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Manager/Manager.h>
#include <Manager/TextureManager.h>

#include <Utils/GPU.h>

SkinnedMesh::SkinnedMesh() {
}


SkinnedMesh::~SkinnedMesh() {
}

bool SkinnedMesh::LoadMesh(const std::string& fileName)
{
	Mesh::Clear();

	Assimp::Importer Importer;

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs;
	if (glPrimitive == GL_TRIANGLES) flags |= aiProcess_Triangulate;

	const aiScene* pScene = Importer.ReadFile(fileName.c_str(), flags);

	if (pScene) {
		return InitFromScene(pScene, fileName);
	}

	printf("Error parsing '%s': '%s'\n", fileName.c_str(), Importer.GetErrorString());
	return false;
}

void SkinnedMesh::Render()
{
	Mesh::Render();
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
	bones.resize(nrVertices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < meshEntries.size(); i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(paiMesh, i);
	}

	if (useMaterial && !InitMaterials(pScene, File))
		return false;

	buffers = UtilsGPU::UploadData<VertexBoneData>(positions, normals, texCoords, bones, indices);
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
			nrBones++;
			BoneInfo bi;
			boneInfo.push_back(bi);
			assimp::CopyMatix(paiMesh->mBones[i]->mOffsetMatrix, boneInfo[boneIndex].BoneOffset);
			skeletalBones[boneName] = boneIndex;
		}
		else {
			boneIndex = skeletalBones[boneName];
		}

		for (unsigned int j = 0; j < paiMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = meshEntries[index]->baseVertex + paiMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = paiMesh->mBones[i]->mWeights[j].mWeight;
			bones[VertexID].AddBoneData(boneIndex, Weight);
		}
	}
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

