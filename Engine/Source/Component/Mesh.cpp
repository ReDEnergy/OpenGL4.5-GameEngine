#include "Mesh.h"

#include <include/utils.h>
#include <include/assimp_utils.h>
#include <include/assimp.h>

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
		this->meshID = meshID;
	meshType = MeshType::STATIC;
	useMaterial = true;
	buffers = new GPUBuffers();
}

Mesh::~Mesh()
{
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
	Assimp::Importer Importer;

	string file = (fileLocation + '/' + fileName).c_str();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_Triangulate;
	const aiScene* pScene = Importer.ReadFile(file, flags);

	if (pScene)
	{
		ClearData();
		this->fileLocation = fileLocation;

		if (InitFromScene(pScene)) {
			return UploadToGPU();
		}
	}

	// pScene is freed when returning because of Importer
	printf("Error parsing '%s': '%s'\n", file.c_str(), Importer.GetErrorString());
	return false;
}

bool Mesh::ConvertMesh(const std::string & fileLocation, const std::string & fileName)
{
	Assimp::Importer Importer;
	Assimp::Exporter exporter;

	string file = (fileLocation + '/' + fileName).c_str();

	unsigned int flags = aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_Triangulate;
	const aiScene* pScene = Importer.ReadFile(file, flags);

	exporter.Export(pScene, "obj", file);
	return true;
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

bool Mesh::InitFromScene(const void* pScene)
{
	auto assimpScene = static_cast<const aiScene*>(pScene);

	meshEntries.resize(assimpScene->mNumMeshes);
	materials.resize(assimpScene->mNumMaterials);

	unsigned int nrVertices = 0;
	unsigned int nrIndices = 0;

	// Count the number of vertices and indices
	for (unsigned int i = 0; i < assimpScene->mNumMeshes; i++) {
		meshEntries[i].materialIndex = assimpScene->mMeshes[i]->mMaterialIndex;
		meshEntries[i].nrIndices = assimpScene->mMeshes[i]->mNumFaces * 3;
		meshEntries[i].baseVertex = nrVertices;
		meshEntries[i].baseIndex = nrIndices;

		nrVertices += assimpScene->mMeshes[i]->mNumVertices;
		nrIndices  += meshEntries[i].nrIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	positions.reserve(nrVertices);
	normals.reserve(nrVertices);
	texCoords.reserve(nrVertices);
	indices.reserve(nrIndices);

	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0 ; i < meshEntries.size() ; i++) {
		const aiMesh* paiMesh = assimpScene->mMeshes[i];
		InitMesh(paiMesh);
	}

	if (useMaterial && !InitMaterials(assimpScene))
		return false;

	return true;
}

void Mesh::InitMesh(const void* paiMesh)
{
	auto assimpMesh = static_cast<const aiMesh*>(paiMesh);

	static const aiVector3D Up(0.0f, 1.0f, 0.0f);
	static const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < assimpMesh->mNumVertices; i++) {
		const aiVector3D* pPos      = &(assimpMesh->mVertices[i]);
		const aiVector3D* pNormal   = assimpMesh->HasNormals()	? &(assimpMesh->mNormals[i]) : &Up;
		const aiVector3D* pTexCoord = assimpMesh->HasTextureCoords(0) ? &(assimpMesh->mTextureCoords[0][i]) : &Zero3D;

		positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		texCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	// Init the index buffer
	for (uint i = 0; i < assimpMesh->mNumFaces; i++)
	{
		const aiFace& Face = assimpMesh->mFaces[i];
		for (uint k = 0; k < Face.mNumIndices; k++)
		{
			indices.push_back(Face.mIndices[k]);
		}
	}

	ComputeBoundingBox();
}

bool Mesh::InitMaterials(const void* pScene)
{
	auto asimpScene = static_cast<const aiScene*>(pScene);

	bool ret = true;
	aiColor4D color;

	for (unsigned int i = 0 ; i < asimpScene->mNumMaterials ; i++) {

		const aiMaterial* pMaterial = asimpScene->mMaterials[i];
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