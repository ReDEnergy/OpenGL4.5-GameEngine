//#include <pch.h>
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

const GLint pixelFormat[5] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
const GLint internalFormat[][5] = {
	{ 0, GL_R8, GL_RG8, GL_RGB8, GL_RGBA8 },
	{ 0, GL_R16, GL_RG16, GL_RGB16, GL_RGBA16 },
	{ 0, GL_R16F, GL_RG16F, GL_RGB16F, GL_RGBA16F },
	{ 0, GL_R32F, GL_RG32F, GL_RGB32F, GL_RGBA32F }
};

Texture::Texture() {
	width = 0;
	height = 0;
	textureID = 0;
}

Texture::~Texture() {
}

GLuint Texture::GetTextureID()
{
	return textureID;
}

bool Texture::Load2D(const char* file_name, GLenum wrapping_mode)
{
	int width, height, chn;
	unsigned char *data = stbi_load(file_name, &width, &height, &chn, 0);

	#ifdef DEBUG_INFO
		cout << width << " * " << height << " channels: " << chn << endl;
	#endif

	if (data == NULL) {
		#ifdef DEBUG_INFO
		cout << "Error loading texture: " << file_name << endl;
		#endif
		return false;
	}

	#ifdef DEBUG_INFO
	cout << "Loaded " << file_name << endl;
	#endif

	Init2DTexture(width, height);

	SetParameters(GL_LINEAR, GL_LINEAR, wrapping_mode);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat[0][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_BYTE, (void*)data);
	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(data);
	return true;
}

void Texture::Create2DTexture(const unsigned char* img, int width, int height, int chn) {
	Init2DTexture(width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat[0][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_BYTE, (void*)img);
	CheckOpenGLError();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Create2DTexture(const unsigned short* img, int width, int height, int chn)
{
	Init2DTexture(width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat[1][chn], width, height, 0, pixelFormat[chn], GL_UNSIGNED_SHORT, (void*)img);
	CheckOpenGLError();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Create2DTextureFloat(const float* data, int width, int height, int chn, int precision /*= 16*/)
{
	int prec = precision == 32 ? 3 : 2;

	Init2DTexture(width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat[prec][chn], width, height, 0, pixelFormat[chn], GL_FLOAT, (void*)data);

	CheckOpenGLError();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::CreateCubeTexture(const float* data, int width, int height, int chn)
{
	this->width = width;
	this->height = height;

	glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//	glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, internalFormat[3][chn], width, height);

	for (int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat[3][chn], width, height, 0, pixelFormat[chn], GL_FLOAT, NULL);
	}

	CheckOpenGLError();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Texture::CreateFrameBufferTexture(int width, int height, int targetID)
{
	Init2DTexture(width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + targetID, GL_TEXTURE_2D, textureID, 0);

	CheckOpenGLError();
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::CreateDepthBufferTexture(int width, int height)
{
	Init2DTexture(width, height);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureID, 0);

	CheckOpenGLError();
	glBindTexture(GL_TEXTURE_2D, 0);
}


void Texture::Bind(GLenum TextureUnit) const
{
	if (textureID) {
		glActiveTexture(TextureUnit);
		glBindTexture(GL_TEXTURE_2D, textureID);
	}
}

void Texture::BindForWriting(GLenum textureTarget) const
{
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, textureID, 0);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
}

void Texture::SetParameters(GLenum mag_filter, GLenum min_filter, GLenum wrapping_mode)
{
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapping_mode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapping_mode);
}

void Texture::GetSize(unsigned int &width,unsigned int &height) const
{
	width = this->width;
	height = this->height;
}

void Texture::Init2DTexture(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;

	glDeleteTextures(1, &textureID);
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	SetParameters(GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE);
}
