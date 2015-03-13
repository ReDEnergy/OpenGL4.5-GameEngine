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
		void Generate(int width, int height, int nr_textures);

		void Bind(bool clearBuffer = true) const;
		void BindTexture(int textureID, GLenum TextureUnit) const;
		void BindAllTextures() const;
		void BindDepthTexture(GLenum TextureUnit) const;

		glm::ivec2 GetResolution() const;

		void SendResolution(Shader *shader) const;
		static void Unbind();
		static void Clear();

	public:
		Texture *textures;
		Texture *depthTexture;

	private:
		GLuint FBO;
		GLenum *DrawBuffers;		// TEST IF IS NECESSARY TO DECLARE
		GLuint depthRenderbuffer;

		GLsizei width;
		GLsizei height;
		GLuint nrTextures;
};