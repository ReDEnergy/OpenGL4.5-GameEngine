#include "Texture.h"

#include <thread>
#include <iostream>

#include <include/gl.h>

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

using uint = unsigned int;
using uchar = unsigned char;

#include <stb/stb_image.h>
#include <stb/stb_image_write.h>

void write_image_thread(const char* fileName, uint width, uint height, uint channels, const uchar *data)
{
	stbi_write_png(fileName, width, height, channels, data, width * channels);
	delete data;
}

const GLint pixelFormat[5] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
const GLint internalFormat[][5] = {
	{ 0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 },
#ifndef OPENGL_ES
	{ 0, GL_R16, GL_RG16, GL_RGB16, GL_RGBA16 },
#else
	{ 0, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F },
#endif
	{ 0, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F },
	{ 0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F }
};

Texture::Texture()
{
	width = 0;
	height = 0;
	channels = 0;
	textureID = 0;
	bitsPerPixel = 8;
	targetType = GL_TEXTURE_2D;

	pixelDataType = GL_UNSIGNED_BYTE;

	wrappingMode = GL_REPEAT;
	textureMinFilter = GL_LINEAR;
	textureMagFilter = GL_LINEAR;
}

Texture::~Texture()
{
	if (textureID)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
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
	pixelDataFormat = pixelFormat[chn];

	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat[0][chn], width, height, 0, pixelDataFormat, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(targetType);
	glBindTexture(targetType, 0);
	CheckOpenGLError();

	stbi_image_free(data);
	return true;
}

void Texture::SaveToFile(const char * fileName) const
{
	#ifndef OPENGL_ES
		unsigned int bpp = bitsPerPixel / 8;
		unsigned char *data = new uchar[width * height * channels * bpp];
		glBindTexture(targetType, textureID);
		glGetTexImage(targetType, 0, pixelDataFormat, pixelDataType, (void*)data);

		stbi_write_png(fileName, width, height, channels, data, width * channels * bpp);
		SAFE_FREE_ARRAY(data);
	#else
		cout << "[OpenGL ES] context does not support glGetTexImage" << endl;
	#endif
	//std::thread([=]() {
	//	stbi_write_png(fileName, width, height, channels, data, width * channels);
	//  delete data;
	//});
}

void Texture::Create2DTexture(uint width, uint height, uint chn, uint bpp, GLenum pixelDataType)
{
	bitsPerPixel = bpp;
	this->pixelDataType = pixelDataType;
	auto iFormat = internalFormat[bpp / 8 - 1][chn];
	pixelDataFormat = pixelFormat[chn];

	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, iFormat, width, height, 0, pixelDataFormat, pixelDataType, nullptr);
	UnBind();
}

void Texture::Create2D(uint width, uint height, uint chn, GLint internalFormat, GLenum format, GLenum pixelDataType)
{
	this->pixelDataFormat = format;
	this->pixelDataType = pixelDataType;

	Init2DTexture(width, height, chn);
	glTexImage2D(targetType, 0, internalFormat, width, height, 0, pixelDataFormat, pixelDataType, nullptr);
	UnBind();
}

void Texture::Create2DTextureFromNative(GLuint textureID, uint width, uint height, uint chn)
{
	this->textureID = textureID;
	this->width = width;
	this->height = height;
}

void Texture::UploadImage(const void * img)
{
	if (!textureID) return;

	SetOptimalPixelUnpacking();

	Bind();
	glTexSubImage2D(targetType, 0, 0, 0, width, height, pixelDataFormat, pixelDataType, img);
	UnBind();
}

void Texture::CreateCubeTexture(const float* data, uint width, uint height, uint chn)
{
	this->width = width;
	this->height = height;
	targetType = GL_TEXTURE_CUBE_MAP;

	glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
#ifndef OPENGL_ES
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
	glBindTexture(targetType, textureID);
	glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internalFormat[3][chn], width, height);

	pixelDataFormat = pixelFormat[chn];

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat[3][chn], width, height, 0, pixelDataFormat, GL_FLOAT, NULL);
	}

	UnBind();
}

void Texture::CreateFrameBufferTexture(uint width, uint height, uint targetID, uint precision /*default is 32*/)
{
	bitsPerPixel = precision;
	pixelDataFormat = pixelFormat[4];
	int prec = precision == 32 / 8 - 1;
	Init2DTexture(width, height, 4);
	glTexImage2D(targetType, 0, internalFormat[prec][4], width, height, 0, pixelDataFormat, GL_UNSIGNED_BYTE, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + targetID, GL_TEXTURE_2D, textureID, 0);
	UnBind();
}

void Texture::CreateDepthBufferTexture(uint width, uint height)
{
	Init2DTexture(width, height, 1);
	glTexImage2D(targetType, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
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

void Texture::BindToTarget(GLenum textureTarget) const
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, textureID, 0);
#ifndef OPENGL_ES
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
#else
#endif
}

void Texture::UnBind() const
{
	glBindTexture(targetType, 0);
	CheckOpenGLError();
}

void Texture::SetTextureParameters()
{
	glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, textureMinFilter);
	glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, textureMagFilter);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_S, wrappingMode);
	glTexParameteri(targetType, GL_TEXTURE_WRAP_T, wrappingMode);
#ifndef OPENGL_ES
	glTexParameterf(targetType, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4);
#endif
}

void Texture::SetOptimalPixelUnpacking()
{
	static const int pack_alignment[4] = { 4, 1, 1, 1 };

	// Set optimal unpacking alignment
	int stride = width * channels * (bitsPerPixel / 8);
	int alignment = pack_alignment[stride % 4];
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
}

unsigned int Texture::GetWidth() const
{
	return width;
}

unsigned int Texture::GetHeight() const
{
	return height;
}

void Texture::GetSize(uint &width, uint &height) const
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
	CheckOpenGLError();
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

		CheckOpenGLError();
	}
}

void Texture::Init2DTexture(uint width, uint height, uint channels)
{
	this->width = width;
	this->height = height;
	this->channels = channels;

	if (textureID)
	{
		glDeleteTextures(1, &textureID);
	}

	glGenTextures(1, &textureID);
	glBindTexture(targetType, textureID);

	SetTextureParameters();

	// Set optimal unpacking alignment
	SetOptimalPixelUnpacking();

	CheckOpenGLError();
}
