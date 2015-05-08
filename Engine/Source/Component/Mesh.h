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

enum class MeshType {
	STATIC,
	MORPHING,
	SKINNED
};

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

/**
 * BoundingBox for original mesh vertices data
 */
class BoundingBox {
	
	public:
		BoundingBox(vector<glm::vec3> &positions);

	public:
		vector<glm::vec3> points;
};

class DLLExport Mesh {
	public:
		Mesh(const char* meshID = NULL);
		virtual ~Mesh();

		bool InitFromData();
		bool InitFromData(vector<glm::vec3>& positions, 
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned short>& indices);

		virtual void Update() {};
		virtual bool LoadMesh(const string& fileLocation, const string& fileName);
		virtual void Render(const Shader *shader);
		virtual void RenderInstanced(unsigned int instances);
		virtual void RenderDebug();
		virtual void UseMaterials(bool);

		void SetGlPrimitive(unsigned int glPrimitive);

	protected:
		void Clear();
		void InitMesh(const aiMesh* paiMesh);
		bool InitMaterials(const aiScene* pScene);
		virtual bool InitFromScene(const aiScene* pScene);

	private:
		string meshID;

	public:
		MeshType meshType;
		glm::vec4 debugColor;
		vector<glm::vec3> positions;
		vector<glm::vec3> normals;
		vector<glm::vec2> texCoords;
		vector<unsigned short> indices;
		BoundingBox *bbox;

	protected:
		string fileLocation;

		bool useMaterial;
		unsigned int glPrimitive;
		GPUBuffers *buffers;

		vector<MeshEntry*> meshEntries;
		vector<Material*> materials;
};