#pragma once
#include <vector>

#include <assimp/Importer.hpp>		// C++ importer interface
#include <assimp/scene.h>			// Output data structure
#include <assimp/postprocess.h>		// Post processing flags

#include <include/gl.h>
#include <include/glm.h>

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
		~Mesh();

		bool LoadMesh(const std::string& fileName);
		bool InitFromData();
		bool InitFromData(vector<glm::vec3>& positions, 
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned short>& indices);
		void Render();
		void RenderInstanced(unsigned int instances);
		void RenderDebug();
		void UseMaterials(bool);
		void SetGlPrimitive(unsigned int glPrimitive);

	private:
		void Clear();
		bool InitFromScene(const aiScene* pScene, const std::string& File);
		void InitMesh(const aiMesh* paiMesh,
					vector<glm::vec3>& positions,
					vector<glm::vec3>& normals,
					vector<glm::vec2>& texCoords,
					vector<unsigned short>& indices);

		bool InitMaterials(const aiScene* pScene, const std::string& File);

	private:
		bool useMaterial;
		unsigned int glPrimitive;
		GPUBuffers *buffers;

		vector<MeshEntry*> meshEntries;
		vector<Material*> materials;

	public:
		glm::vec4 debugColor;
		vector<glm::vec3> positions;
		vector<glm::vec3> normals;
		vector<glm::vec2> texCoords;
		vector<unsigned short> indices;
};

inline void color_to_vec4(const aiColor4D &color, glm::vec4 &dest)
{
	dest.r = color.r;
	dest.g = color.g;
	dest.b = color.b;
	dest.a = color.a;
}