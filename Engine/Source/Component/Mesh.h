#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/dll_export.h>
#include <include/gl.h>
#include <include/glm.h>

using namespace std;

class Shader;
class Texture;
class Material;
class GPUBuffers;

static const unsigned int INVALID_MATERIAL = 0xFFFFFFFF;

enum class MeshType
{
	STATIC,
	MORPHING,
	SKINNED
};

enum class LOG_MESH
{
	ERROR_MAX_INFLUENCE,
	SUCCESS
};

struct MeshEntry
{
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
class BoundingBox
{
	
	public:
		BoundingBox(vector<glm::vec3> &positions);

	public:
		vector<glm::vec3> points;
};

class DLLExport Mesh
{
	public:
		Mesh(const char* meshID = NULL);
		virtual ~Mesh();

		void ClearData();
		bool InitFromData();
		bool InitFromData(vector<glm::vec3>& positions, 
						vector<glm::vec3>& normals,
						vector<glm::vec2>& texCoords,
						vector<unsigned short>& indices);

		virtual void Update() {};
		virtual bool LoadMesh(const string& fileLocation, const string& fileName);
		virtual void Render(const Shader *shader);
		virtual void RenderInstanced(unsigned int instances);
		virtual void RenderDebug(const Shader *shader) const;
		virtual void UseMaterials(bool);

		void SetGlPrimitive(unsigned int glPrimitive);
		void SetCulling(bool value = true);
		const char* GetMeshID() const;

	protected:
		#ifdef ENGINE_DLL_EXPORTS
		virtual void InitMesh(const aiMesh* paiMesh);
		virtual bool InitMaterials(const aiScene* pScene);
		virtual bool InitFromScene(const aiScene* pScene);
		#endif
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
		LOG_MESH loadState;
		string fileLocation;

		bool useMaterial;
		unsigned int glPrimitive;
		GPUBuffers *buffers;

		vector<MeshEntry*> meshEntries;
		vector<Material*> materials;
};