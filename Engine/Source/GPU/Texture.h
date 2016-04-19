#pragma once
#include <include/gl_defines.h>
#include <include/dll_export.h>

class DLLExport Texture
{
	public:
		Texture();
		~Texture();

		void Bind() const;
		void BindToTextureUnit(GLenum TextureUnit) const;
		void BindForWriting(GLenum textureTarget) const;
		void UnBind() const;

		void UploadNewData(const unsigned char* img);

		void Create2DTexture(const unsigned char* img, int width, int height, int chn);
		void Create2DTexture(const unsigned short* img, int width, int height, int chn);
		void CreateCubeTexture(const float* data, int width, int height, int chn);
		void Create2DTextureFloat(const float* data, int width, int height, int chn, int precision = 16);
		void CreateFrameBufferTexture(int width, int height, int targetID, int precision = 32);
		void CreateDepthBufferTexture(int width, int height);

		bool Load2D(const char* fileName, GLenum wrappingMode = GL_REPEAT);
		void SaveToFile(const char* fileName) const;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		void GetSize(unsigned int &width, unsigned int &height) const;

		void SetWrappingMode(GLenum mode);
		void SetFiltering(GLenum minFilter, GLenum magFilter = GL_LINEAR);
		void SetTextureData(const unsigned char* img) const;

		GLuint GetTextureID() const;

	private:
		void SetTextureParameters();
		void Init2DTexture(unsigned int width, unsigned int height, unsigned int channels);

	private:
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		GLuint targetType;
		GLuint textureID;
		GLenum wrappingMode;
		GLenum textureMinFilter;
		GLenum textureMagFilter;
};
