#pragma once

#include <vector>

#include <include/dll_export.h>
#include <include/glm.h>
#include <include/gl_defines.h>

class Shader;
class Texture;
class WindowObject;

class DLLExport FrameBuffer
{
	public:
		FrameBuffer();
		~FrameBuffer();
		void Clean();
		void Generate(int width, int height, int nrTextures, bool hasDepthTexture = true, int precision = 32);

		void Bind(bool clearBuffer = true) const;
		void BindTexture(int textureID, unsigned int TextureUnit) const;
		void BindAllTextures() const;
		void BindDepthTexture(unsigned int TextureUnit) const;

		Texture* GetTexture(unsigned int index) const;
		Texture* GetDepthTexture() const;
		GLuint GetTextureID(unsigned int index) const;
		unsigned int GetNumberOfRenderTargets() const;

		glm::ivec2 GetResolution() const;
		void SendResolution(Shader *shader) const;

		static void Unbind(WindowObject *window);
		static void Clear();
		static void SetOffScreenBuffer(FrameBuffer *buffer);
		static FrameBuffer* GetOffScreenBuffer();

	private:
		static FrameBuffer *OffScreenBuffer;

		Texture *textures;
		Texture *depthTexture;

		unsigned int FBO;
		unsigned int *DrawBuffers;		// TEST IF IS NECESSARY TO DECLARE
		unsigned int depthRenderbuffer;

		int width;
		int height;
		unsigned int nrTextures;
};