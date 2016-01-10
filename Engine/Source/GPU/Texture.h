#pragma once
#include <include/gl.h>
#include <include/dll_export.h>

using namespace std;

enum class WRAPPING_MODE : UINT32
{
	REPEAT = GL_REPEAT,
	CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	MIRRORED_REPEAT = GL_MIRRORED_REPEAT
};

enum class TEXTURE_TYPE {
	NORMAL_TEXTURE,
	CUBE_TEXTURE
};

class DLLExport Texture
{
	public:
		Texture();
		~Texture();

		void Bind() const;
		void BindToTextureUnit(GLenum TextureUnit) const;
		void BindForWriting(GLenum textureTarget) const;
		void UnBind() const;

		void Create2DTexture(const unsigned char* img, int width, int height, int chn);
		void Create2DTexture(const unsigned short* img, int width, int height, int chn);
		void CreateCubeTexture(const float* data, int width, int height, int chn);
		void Create2DTextureFloat(const float* data, int width, int height, int chn, int precision = 16);
		void CreateFrameBufferTexture(int width, int height, int target_id);
		void CreateDepthBufferTexture(int width, int height);

		bool Load2D(const char* fileName, GLenum wrappingMode = GL_REPEAT);
		void SaveToFile(const char* fileName) const;

		unsigned int GetWidth() const;
		unsigned int GetHeight() const;
		void GetSize(unsigned int &width, unsigned int &height) const;

		void SetWrappingMode(GLenum mode);
		void SetFiltering(GLenum filtering);
		void SetTextureData(const unsigned char* img) const;

		GLuint GetTextureID() const;

	private:
		void Init2DTexture(unsigned int width, unsigned int height, unsigned int channels);
		void SetParameters(GLenum mag_filter, GLenum min_filter, GLenum wrapping_mode);

	private:
		unsigned int width;
		unsigned int height;
		unsigned int channels;
		GLuint targetType;
		GLuint textureID;
		GLenum wrappingMode;
		GLenum textureFiltering;
};