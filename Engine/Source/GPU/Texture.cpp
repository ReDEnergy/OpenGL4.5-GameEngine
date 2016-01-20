//#include <pch.h>
#include "Texture.h"

#include <thread>
#include <iostream>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

using uint = unsigned int;
using uchar = unsigned char;

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

const GLint pixelFormat[5] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
const GLint internalFormat[][5] = {
	{ 0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 },
	{ 0, GL_R16, GL_RG16, GL_RGB16, GL_RGBA16 },
	{ 0, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F },
	{ 0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F }
};

Texture::Texture()
{
	width = 0;
	height = 0;
	channels = 0;
	textureID = 0;
	targetType = GL_TEXTURE_2D;
	wrappingMode = GL_REPEAT;
	textureMinFilter = GL_LINEAR;
	textureMagFilter = GL_LINEAR;
}

Texture::~Texture() {
}

GLuint Texture::GetTextureID() const
{
	return textureID;
}

bool Texture::Load2D(const char* fileName, GLenum wrapping_mode)
{
	int width, height, chn;
	unsigned char *data = stbi_load(fileName, &width, &height, &chn, 0);

	if (data == NULL) {
		#ifdef DEBUG_INFO
		cout << "ERROR loading texture: " << file_name << endl << endl;
		#endif
		return false;
	}

	#ifdef DEBUG_INFO
	cout << "Loaded " << file_name << endl;
	cout << width << " * " << height << " channels: " << chn << endl << endl;
	#endif

	textureMinFilter = GL_LINEAR_MIPMAP_LINEAR;
	wrappingMode = wrapping_mode;

	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat[0][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(targetType);
	glBindTexture(targetType, 0);
	CheckOpenGLError();

	stbi_image_free(data);
	return true;
}

void write_image_thread(const char* fileName, uint width, uint height, uint channels, const uchar *data)
{
	stbi_write_png(fileName, width, height, channels, data, width * channels);
	delete data;
}

int global_counter = 0;

void increase_global(int n) { for (int i = 0; i<n; ++i) ++global_counter; }

void Texture::SaveToFile(const char * fileName) const
{
	unsigned char *data = new unsigned char[width * height * channels];
	glBindTexture(targetType, textureID);
	glGetTexImage(targetType, 0, pixelFormat[channels], GL_UNSIGNED_BYTE, (void*)data);

	stbi_write_png(fileName, width, height, channels, data, width * channels);
	delete data;
	//std::thread([=]() {
	//	stbi_write_png(fileName, width, height, channels, data, width * channels);
	//  delete data;
	//});
}

void Texture::Create2DTexture(const unsigned char* img, int width, int height, int chn)
{
	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat[0][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_BYTE, (void*)img);
	UnBind();
}

void Texture::Create2DTexture(const unsigned short* img, int width, int height, int chn)
{
	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat[1][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_SHORT, (void*)img);
	UnBind();
}

void Texture::Create2DTextureFloat(const float* data, int width, int height, int chn, int precision /*= 16*/)
{
	int prec = precision == 32 ? 3 : 2;
	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat[prec][chn], width, height, 0, pixelFormat[chn], GL_FLOAT, (void*)data);
	UnBind();
}

void Texture::CreateCubeTexture(const float* data, int width, int height, int chn)
{
	this->width = width;
	this->height = height;
	targetType = GL_TEXTURE_CUBE_MAP;

	glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindTexture(targetType, textureID);
	glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internalFormat[3][chn], width, height);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat[3][chn], width, height, 0, pixelFormat[chn], GL_FLOAT, NULL);
	}

	UnBind();
}

void Texture::CreateFrameBufferTexture(int width, int height, int targetID)
{
	Init2DTexture(width, height, 4);
	glTexImage2D(targetType, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + targetID, GL_TEXTURE_2D, textureID, 0);
	UnBind();
}

void Texture::CreateDepthBufferTexture(int width, int height)
{
	Init2DTexture(width, height, 1);
	glTexImage2D(targetType, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);
	UnBind();
}


void Texture::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::BindToTextureUnit(GLenum TextureUnit) const
{
	if (!textureID) return;
	glActiveTexture(TextureUnit);
	glBindTexture(GL_TEXTURE_2D, textureID);
}

void Texture::UnBind() const
{
	glBindTexture(targetType, 0);
	CheckOpenGLError();
}

void Texture::BindForWriting(GLenum textureTarget) const
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, textureID, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void Texture::SetTextureParameters()
{
	glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_T, wrappingMode);
	glTexParameterf(targetType, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
}

unsigned int Texture::GetWidth() const
{
	return width;
}

unsigned int Texture::GetHeight() const
{
	return height;
}

void Texture::GetSize(unsigned int &width,unsigned int &height) const
{
	width = this->width;
	height = this->height;
}

void Texture::SetWrappingMode(GLenum mode)
{
	if (wrappingMode == mode)
		return;

	wrappingMode = mode;

	if (textureID)
	{
		glBindTexture(targetType, textureID);
		glTexParameteri(targetType, GL_TEXTURE_WRAP_S, mode);
		glTexParameteri(targetType, GL_TEXTURE_WRAP_T, mode);
		glTexParameteri(targetType, GL_TEXTURE_WRAP_R, mode);
	}
}

void Texture::SetFiltering(GLenum minFilter, GLenum magFilter)
{
	if (textureID)
	{
		glBindTexture(targetType, textureID);

		if (textureMinFilter != minFilter) {
			glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, minFilter);
			textureMinFilter = minFilter;
		}

		if (textureMagFilter != magFilter) {
			glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, magFilter);
			textureMagFilter = magFilter;
		}
	}
}

void Texture::SetTextureData(const unsigned char * img) const
{
	glBindTexture(targetType, textureID);
	glTexImage2D(targetType, 0, internalFormat[0][channels], width, height, 0, pixelFormat[channels], GL_UNSIGNED_BYTE, (void*)img);
	UnBind();
}

void Texture::Init2DTexture(unsigned int width, unsigned int height, unsigned int channels)
{
	this->width = width;
	this->height = height;
	this->channels = channels;

	CheckOpenGLError();
	if (textureID)
		glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
	glBindTexture(targetType, textureID);
	SetTextureParameters();
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	CheckOpenGLError();
}
