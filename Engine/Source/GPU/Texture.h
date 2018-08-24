#pragma once
#include <include/gl_defines.h>
#include <include/dll_export.h>
#include <include/utils.h>

class DLLExport Texture
{
	typedef unsigned int uint;
	typedef unsigned char uchar;
	typedef unsigned short ushort;

	public:
		Texture();
		~Texture();

		void Bind() const;
		void BindToTarget(GLenum textureTarget) const;
		void BindToTextureUnit(GLenum TextureUnit) const;
		void UnBind() const;

		/**
		Create a GPU texture with the specified parameters
		@param width, height - size of the texture
		@param bpp - bits per pixel
		@param dataType - specifies the data type of the pixel data
			- GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT, etc
			- full list: https://www.opengl.org/sdk/docs/man/html/glTexImage2D.xhtml
		*/
		void Create2DTexture(uint width, uint height, uint chn, uint bpp = 8, GLenum pixelDataType = GL_UNSIGNED_BYTE);
		void Create2D(uint width, uint height, uint chn, GLint internalFormat = GL_RGBA8, GLenum format = GL_RGBA, GLenum pixelDataType = GL_UNSIGNED_BYTE);

		void Create2DTextureFromNative(GLuint textureID, uint width, uint height, uint chn);
		void CreateCubeTexture(const float* data, uint width, uint height, uint chn);
		void CreateFrameBufferTexture(uint width, uint height, uint targetID, uint precision = 32);
		void CreateDepthBufferTexture(uint width, uint height);

		void UploadImage(const void *img);

		bool Load2D(const char* fileName, GLenum wrappingMode = GL_REPEAT);
		void SaveToFile(const char* fileName) const;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		void GetSize(uint &width, uint &height) const;

		void SetWrappingMode(GLenum mode);
		void SetFiltering(GLenum minFilter, GLenum magFilter = GL_LINEAR);

		GLuint GetTextureID() const;

	private:
		void SetTextureParameters();
		void SetOptimalPixelUnpacking();
		void Init2DTexture(uint width, uint height, uint channels);

	private:
		uint width;
		uint height;
		uint channels;
		uint bitsPerPixel;

		GLuint textureID;

		// GL_TEXTURE_2D
		GLuint targetType;
		GLenum pixelDataFormat;

		// GL_UNSIGNED_BYTE, GL_BYTE, GL_UNSIGNED_SHORT, GL_SHORT, GL_UNSIGNED_INT, GL_INT, GL_FLOAT
		GLenum pixelDataType;
		GLenum wrappingMode;
		GLenum textureMinFilter;
		GLenum textureMagFilter;
};
