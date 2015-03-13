#pragma once
#include <include/gl.h>
#include <include/dll_export.h>

using namespace std;

class DLLExport Texture
{
	public:
		Texture();
		~Texture();

		void Bind(GLenum TextureUnit) const;
		void BindForWriting(GLenum textureTarget) const;

		void Create2DTexture(const unsigned char* img, int width, int height, int chn);
		void Create2DTexture(const unsigned short* img, int width, int height, int chn);
		void CreateCubeTexture(const float* data, int width, int height, int chn);
		void Create2DTextureFloat(const float* data, int width, int height, int chn, int precision = 16);
		void CreateFrameBufferTexture(int width, int height, int target_id);
		void CreateDepthBufferTexture(int width, int height);

		void GetSize(unsigned int &width,unsigned int &height) const;
		bool Load2D(const char* file_name, GLenum wrapping_mode = GL_REPEAT);
		GLuint GetTextureID();

	private:
		void Init2DTexture(unsigned int width, unsigned int height);
		void SetParameters(GLenum mag_filter, GLenum min_filter, GLenum wrapping_mode);
	private:
		unsigned int width;
		unsigned int height;
		GLuint textureID;
};

enum MIN_FILTER {
	MIN_NEAREST = GL_NEAREST,
	MIN_LINEAR = GL_LINEAR,
	MIN_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
	MIN_NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
	MIN_LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
	MIN_LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
};

enum MAG_FILTER {
	MAG_NEAREST = GL_NEAREST,
	MAG_LINEAR = GL_LINEAR
};

enum WRAPPING_MODE {
	WM_REPEAT = GL_REPEAT,
	WM_CLAMP_TO_EDGE = GL_CLAMP_TO_EDGE,
	WM_MIRRORED_REPEAT = GL_MIRRORED_REPEAT
};

enum TEXTURE_TYPE {
	NORMAL_TEXTURE,
	CUBE_TEXTURE
};