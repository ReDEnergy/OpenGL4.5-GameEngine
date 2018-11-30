#pragma once
#include <vector>

#ifdef ENGINE_DLL_EXPORTS
#include <include/assimp.h>
#endif

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/gl_defines.h>

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

enum class MeshStatus
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
	unsigned int nrIndices;
	unsigned int baseVertex;
	unsigned int baseIndex;
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
						std::vector<unsigned int>& indices);

		virtual void Update() {};
		virtual bool LoadMesh(const std::string& fileLocation, const std::string& fileName);
		virtual bool ConvertMesh(const std::string& fileLocation, const std::string& fileName);
		virtual void UseMaterials(bool);

		const char* GetMeshID() const;

		glm::vec3 GetCenterPoint() const;
		glm::vec3 GetHalfSize() const;

	protected:
		void ComputeBoundingBox();
		virtual bool UploadToGPU();

		bool InitMaterials(const void* pScene);
		virtual void InitMesh(const void* paiMesh);
		virtual bool InitFromScene(const void* pScene);

	private:
		std::string meshID;
		glm::vec3 halfSize;
		glm::vec3 meshCenter;

	public:
		MeshType meshType;
		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<unsigned int> indices;

	protected:
		MeshStatus loadState;
		std::string fileLocation;

		bool useMaterial;
		GLenum glDrawMode;
		GPUBuffers *buffers;

		std::vector<MeshEntry> meshEntries;
		std::vector<Material*> materials;
};