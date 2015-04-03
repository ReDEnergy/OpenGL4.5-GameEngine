#pragma once
#include <vector>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/gl.h>
#include <include/glm.h>
#include <include/dll_export.h>

using namespace std;

class Shader;
class Texture;
class Material;
class GPUBuffers;

static const unsigned int INVALID_MATERIAL = 0xFFFFFFFF;

struct MeshEntry {
	MeshEntry()
	{
		nrIndices = 0;
		baseVertex = 0;
		baseIndex = 0;
		materialIndex = INVALID_MATERIAL;
	}
	unsigned short nrIndices;
	unsigned short baseVertex;
	unsigned short baseIndex;
	unsigned int materialIndex;
};

class Mesh {
	public:
		Mesh();
		virtual ~Mesh();

		bool InitFromData();
		bool InitFromData(vector<glm::vec3>& positions, 
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned short>& indices);

		virtual bool LoadMesh(const std::string& fileName);
		virtual void Render();
		virtual void RenderInstanced(unsigned int instances);
		virtual void RenderDebug();
		virtual void UseMaterials(bool);

		void SetGlPrimitive(unsigned int glPrimitive);

	protected:
		void Clear();
		virtual bool InitFromScene(const aiScene* pScene, const std::string& File);
		void InitMesh(const aiMesh* paiMesh);
		bool InitMaterials(const aiScene* pScene, const std::string& File);

	public:
		glm::vec4 debugColor;
		vector<glm::vec3> positions;
		vector<glm::vec3> normals;
		vector<glm::vec2> texCoords;
		vector<unsigned short> indices;

	protected:
		bool useMaterial;
		unsigned int glPrimitive;
		GPUBuffers *buffers;

		vector<MeshEntry*> meshEntries;
		vector<Material*> materials;
};