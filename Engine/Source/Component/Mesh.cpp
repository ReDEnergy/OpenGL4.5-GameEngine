#include "Mesh.h"

#include <include/utils.h>
#include <include/assimp_utils.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/TextureManager.h>

#include <Utils/GPU.h>

using namespace std;

Mesh::Mesh(const char* meshID)
{
	if (meshID)
		this->meshID.assign(meshID);
	meshType = MeshType::STATIC;
	useMaterial = true;
	buffers = new GPUBuffers();
}

Mesh::~Mesh() {
	ClearData();
	meshEntries.clear();
	SAFE_FREE(buffers);
}


void Mesh::ClearData()
{
	loadState = MeshStatus::SUCCESS;
	for (unsigned int i = 0 ; i < materials.size() ; i++) {
		SAFE_FREE(materials[i]);
	}
	positions.clear();
	texCoords.clear();
	indices.clear();
	normals.clear();
}


bool Mesh::LoadMesh(const string& fileLocation, const string& fileName)
{
	ClearData();
	this->fileLocation = fileLocation;
	string file = (fileLocation + '/' + fileName).c_str();

	Assimp::Importer Importer;

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_Triangulate;
	const aiScene* pScene = Importer.ReadFile(file, flags);

	if (pScene) {
		if (InitFromScene(pScene)) {
			return UploadToGPU();
		}
	}

	// pScene is freed when returning because of Importer

	printf("Error parsing '%s': '%s'\n", file.c_str(), Importer.GetErrorString());
	return false;
}

bool Mesh::UploadToGPU()
{
	buffers->ReleaseMemory();

	if (texCoords.size())
	{
		*buffers = UtilsGPU::UploadData(positions, normals, texCoords, indices);
	}
	else
	{
		*buffers = UtilsGPU::UploadData(positions, normals, indices);
	}

	return buffers->VAO != 0;
}

bool Mesh::InitFromData()
{
	meshEntries.clear();

	MeshEntry M;
	M.nrIndices = static_cast<unsigned int>(indices.size());
	meshEntries.push_back(M);

	ComputeBoundingBox();
	return UploadToGPU();
}

bool Mesh::InitFromData(vector<glm::vec3>& positions,
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned int>& indices)
{
	this->positions = positions;
	this->normals = normals;
	this->texCoords = texCoords;
	this->indices = indices;

	return InitFromData();
}

bool Mesh::InitFromScene(const aiScene* pScene)
{
	meshEntries.resize(pScene->mNumMeshes);
	materials.resize(pScene->mNumMaterials);

	unsigned int nrVertices = 0;
	unsigned int nrIndices = 0;

	// Count the number of vertices and indices
	for (unsigned int i = 0; i < pScene->mNumMeshes; i++) {
		meshEntries[i].materialIndex = pScene->mMeshes[i]->mMaterialIndex;
		meshEntries[i].nrIndices = pScene->mMeshes[i]->mNumFaces * 3;
		meshEntries[i].baseVertex = nrVertices;
		meshEntries[i].baseIndex = nrIndices;

		nrVertices += pScene->mMeshes[i]->mNumVertices;
		nrIndices  += meshEntries[i].nrIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	positions.reserve(nrVertices);
	normals.reserve(nrVertices);
	texCoords.reserve(nrVertices);
	indices.reserve(nrIndices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0 ; i < meshEntries.size() ; i++) {
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(paiMesh);
	}

	if (useMaterial && !InitMaterials(pScene))
		return false;

	return true;
}

void Mesh::InitMesh(const aiMesh* paiMesh)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
		const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		texCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	// Init the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
		const aiFace& Face = paiMesh->mFaces[i];
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);
		if (Face.mNumIndices == 4)
			indices.push_back(Face.mIndices[3]);
	}

	ComputeBoundingBox();
}

bool Mesh::InitMaterials(const aiScene* pScene)
{
	bool ret = true;
	aiColor4D color;

	for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {

		const aiMaterial* pMaterial = pScene->mMaterials[i];
		materials[i] = new Material();

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString Path;
			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
				materials[i]->texture = Manager::Texture->LoadTexture(fileLocation, Path.data);
			}
		}
		if (aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &color) == AI_SUCCESS)
			assimp::CopyColor(color, materials[i]->ambient);

		if (aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &color) == AI_SUCCESS)
			assimp::CopyColor(color, materials[i]->diffuse);

		if (aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &color) == AI_SUCCESS)
			assimp::CopyColor(color, materials[i]->specular);

		if (aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_EMISSIVE, &color) == AI_SUCCESS)
			assimp::CopyColor(color, materials[i]->emissive);

		//unsigned int max;
		//if (aiGetMaterialFloatArray(pMaterial, AI_MATKEY_SHININESS, &materials[i]->shininess, &max) == AI_SUCCESS) { };
	}

	CheckOpenGLError();

	return ret;
}

void Mesh::UseMaterials(bool value)
{
	useMaterial = value;
}

//void Mesh::SetGLDrawMode(GLenum drawMode)
//{
//	glDrawMode = drawMode;
//}

const char * Mesh::GetMeshID() const
{
	return meshID.c_str();
}

glm::vec3 Mesh::GetCenterPoint() const
{
	return meshCenter;
}

glm::vec3 Mesh::GetHalfSize() const
{
	return halfSize;
}

void Mesh::ComputeBoundingBox()
{
	glm::vec3 maxValue = positions[0];
	glm::vec3 minValue = positions[0];

	for (auto point : positions) {
		// find max
		if (point.x > maxValue.x)	maxValue.x = point.x;
		if (point.y > maxValue.y)	maxValue.y = point.y;
		if (point.z > maxValue.z)	maxValue.z = point.z;

		// find min
		if (point.x < minValue.x)	minValue.x = point.x;
		if (point.y < minValue.y)	minValue.y = point.y;
		if (point.z < minValue.z)	minValue.z = point.z;
	}

	halfSize = (maxValue - minValue) / 2.0f;
	meshCenter = (maxValue + minValue) / 2.0f;
}