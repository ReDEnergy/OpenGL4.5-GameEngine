#pragma once

#include <vector>

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/gl.h>

class Shader;
class Texture;

using namespace std;

class DLLExport FrameBuffer
{
	public:
		FrameBuffer();
		~FrameBuffer();
		void Clean();
		void Generate(int width, int height, int nr_textures, bool depthTexture = true);

		void Bind(bool clearBuffer = true) const;
		void BindTexture(int textureID, GLenum TextureUnit) const;
		void BindAllTextures() const;
		void BindDepthTexture(GLenum TextureUnit) const;

		Texture* GetTexture(unsigned int index) const;
		Texture* GetDepthTexture() const;
		GLuint GetTextureID(unsigned int index) const;
		unsigned int GetNumberOfRenderTargets() const;

		glm::ivec2 GetResolution() const;
		void SendResolution(Shader *shader) const;

		static void Unbind();
		static void Clear();

	private:
		Texture *textures;
		Texture *depthTexture;

		GLuint FBO;
		GLenum *DrawBuffers;		// TEST IF IS NECESSARY TO DECLARE
		GLuint depthRenderbuffer;

		GLsizei width;
		GLsizei height;
		GLuint nrTextures;
};