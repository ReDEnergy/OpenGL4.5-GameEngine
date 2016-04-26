#pragma once
#include <vector>

#include <include/dll_export.h>

class Shader;
class Mesh;

class DLLExport MeshRenderer
{
	public:
		MeshRenderer(Mesh& mesh);
		MeshRenderer(MeshRenderer& meshRenderer);
		virtual ~MeshRenderer();

		virtual void Render(const Shader *shader) const;
		virtual void RenderInstanced(unsigned int instances) const;
		virtual void RenderDebug(const Shader *shader) const;
		virtual void UseMaterials(bool);

		void InitForNewContext();
		void SetGLDrawMode(unsigned int drawMode);
		void SetCulling(bool value = true);

	public:
		Mesh *mesh;

	protected:
		bool useMaterial;
		unsigned int glDrawMode;
		int glVAO;
};