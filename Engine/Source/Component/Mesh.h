#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif
#include <include/dll_export.h>
#include <include/glm.h>

class Shader;
class Texture;
class Material;
class GPUBuffers;

static const unsigned int INVALID_MATERIAL = 0xFFFFFFFF;

enum class MESH_TYPE
{
	STATIC,
	MORPHING,
	SKINNED
};

enum class MESH_STATUS
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

class DLLExport Mesh
{
	friend class MeshRenderer;

	public:
		Mesh(const char* meshID = NULL);
		virtual ~Mesh();

		void ClearData();
		bool InitFromData();
		bool InitFromData(std::vector<glm::vec3>& positions,
						std::vector<glm::vec3>& normals,
						std::vector<glm::vec2>& texCoords,
						std::vector<unsigned short>& indices);

		virtual void Update() {};
		virtual bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		virtual void UseMaterials(bool);

		void SetGLDrawMode(unsigned int drawMode);
		void SetCulling(bool value = true);
		const char* GetMeshID() const;

		glm::vec3 GetCenterPoint() const;
		glm::vec3 GetHalfSize() const;

	protected:
		void ComputeBoundingBox();

		#ifdef ENGINE_DLL_EXPORTS
		virtual void InitMesh(const aiMesh* paiMesh);
		virtual bool InitMaterials(const aiScene* pScene);
		virtual bool InitFromScene(const aiScene* pScene);
		#endif

	private:
		std::string meshID;
		glm::vec3 halfSize;
		glm::vec3 meshCenter;

	public:
		MESH_TYPE meshType;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned short> indices;

	protected:
		MESH_STATUS loadState;
		std::string fileLocation;

		bool useMaterial;
		unsigned int glDrawMode;
		GPUBuffers *buffers;

		std::vector<MeshEntry> meshEntries;
		std::vector<Material*> materials;
};