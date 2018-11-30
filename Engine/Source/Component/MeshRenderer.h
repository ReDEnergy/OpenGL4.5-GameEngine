#pragma once
#include <vector>

#include <include/dll_export.h>
#include <include/gl_defines.h>

class Shader;
class Mesh;

class DLLExport MeshRenderer
{
	public:
		MeshRenderer(const Mesh& mesh);
		MeshRenderer(MeshRenderer& meshRenderer);
		void InitFromMesh(const Mesh& mesh);
		virtual ~MeshRenderer();

		virtual void Render() const;
		virtual void RenderInstanced(unsigned int instances) const;
		virtual void UseMaterials(bool);

		void InitForNewContext();
		void SetGLDrawMode(GLenum drawMode);
		bool IsValid() const;

	public:
		const Mesh *mesh;

	protected:
		bool useMaterial;
		unsigned int glDrawMode;
		int glVAO;
};