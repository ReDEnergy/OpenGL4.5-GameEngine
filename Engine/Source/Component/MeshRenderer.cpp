#include "MeshRenderer.h"

#include <include/utils.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/Material.h>

#include <Component/Mesh.h>

#include <Manager/Manager.h>
#include <Manager/ResourceManager.h>
#include <Manager/RenderingSystem.h>
#include <Manager/TextureManager.h>

#include <Utils/GPU.h>

using namespace std;

MeshRenderer::MeshRenderer(Mesh& mesh)
{
	this->mesh = &mesh;
	useMaterial = mesh.useMaterial;
	glDrawMode = GL_TRIANGLES;
	glVAO = mesh.buffers->VAO;
}

MeshRenderer::MeshRenderer(MeshRenderer & meshRenderer)
{
	this->mesh = meshRenderer.mesh;
	useMaterial = meshRenderer.useMaterial;
	glDrawMode = meshRenderer.glDrawMode;
	glVAO = meshRenderer.glVAO;
}

MeshRenderer::~MeshRenderer()
{}

void MeshRenderer::UseMaterials(bool value)
{
	useMaterial = value;
}

void MeshRenderer::InitForNewContext()
{
	glVAO = mesh->buffers->CreateNewVAO();
}

void MeshRenderer::SetGLDrawMode(GLenum drawMode)
{
	glDrawMode = drawMode;
}

void MeshRenderer::Render() const
{
	glBindVertexArray(glVAO);
	for (unsigned int i = 0 ; i < mesh->meshEntries.size() ; i++) {

		if (useMaterial) {
			const unsigned int materialIndex = mesh->meshEntries[i].materialIndex;
			if (materialIndex != INVALID_MATERIAL && mesh->materials[materialIndex]->texture) {
				(mesh->materials[materialIndex]->texture)->BindToTextureUnit(GL_TEXTURE0);
				// TODO: RenderDoc will crash here
				//glBindBufferBase(GL_UNIFORM_BUFFER, 0, materials[materialIndex]->material_ubo);
			}
			else {
				Manager::Texture->GetTexture(static_cast<unsigned int>(0))->BindToTextureUnit(GL_TEXTURE0);
			}
		}

		#ifdef OPENGL_ES
		glDrawElements(glDrawMode, mesh->meshEntries[i]->nrIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->meshEntries[i]->baseIndex));
		if (mesh->meshEntries[i]->baseVertex) {
			printf("[ERROR]\t Base Vertex different than 0... some part of the mesh might not be visible\n");
		}
		#else
		glDrawElementsBaseVertex(glDrawMode, mesh->meshEntries[i].nrIndices,
								GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->meshEntries[i].baseIndex),
								mesh->meshEntries[i].baseVertex);
		#endif
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	glBindVertexArray(0);
	CheckOpenGLError();
}

void MeshRenderer::RenderInstanced(unsigned int instances) const
{
	instances = instances < 1 ? 1 : instances;
	glBindVertexArray(glVAO);
	for (unsigned int i = 0; i < mesh->meshEntries.size(); i++) {

		if (useMaterial) {
			const unsigned int materialIndex = mesh->meshEntries[i].materialIndex;
			if (materialIndex != INVALID_MATERIAL && mesh->materials[materialIndex]->texture) {
				(mesh->materials[materialIndex]->texture)->BindToTextureUnit(GL_TEXTURE0);
				glBindBufferBase(GL_UNIFORM_BUFFER, 0, mesh->materials[materialIndex]->material_ubo);
			}
			else {
				Manager::Texture->GetTexture(static_cast<unsigned int>(0))->BindToTextureUnit(GL_TEXTURE0);
			}
		}

		#ifdef OPENGL_ES
		glDrawElementsInstanced(glPrimitive, mesh->meshEntries[i]->nrIndices, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * mesh->meshEntries[i]->baseIndex),	instances);
		if (mesh->meshEntries[i]->baseVertex) {
			printf("[ERROR]\t Base Vertex different than 0... some part of the mesh might not be visible\n");
		}
		#else
		glDrawElementsInstancedBaseVertex(glDrawMode, mesh->meshEntries[i].nrIndices, GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int) * mesh->meshEntries[i].baseIndex),
			instances, mesh->meshEntries[i].baseVertex);
		#endif
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	glBindVertexArray(0);
	CheckOpenGLError();
}
