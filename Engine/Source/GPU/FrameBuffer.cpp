//#include <pch.h>
#include "FrameBuffer.h"

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <Core/Engine.h>
#include <Core/WindowObject.h>

#include <include/utils.h>

FrameBuffer::FrameBuffer() {
	FBO = 0; 
	depthTexture = new Texture();
	textures = nullptr;
	DrawBuffers = nullptr;
}

FrameBuffer::~FrameBuffer() {
	SAFE_FREE(depthTexture);
}

void FrameBuffer::Clean() {
	if (FBO)
		glDeleteFramebuffers(1, &FBO);
	SAFE_FREE_ARRAY(textures);
	SAFE_FREE_ARRAY(DrawBuffers)
}

void FrameBuffer::Generate(int width, int height, int nrTextures) {
	
	// clean previous FBO
	Clean();

	#ifdef DEBUG_INFO
		cout << "FBO: " << width << " * " << height << " textures attached: " << nrTextures << endl;
	#endif

	this->width = width;
	this->height = height;
	this->nrTextures = nrTextures;

	// Create FrameBufferObject
	glGenFramebuffers (1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	if (nrTextures > 0) {
		DrawBuffers = new GLenum[nrTextures];

		// Add attachments to drawing buffer
		for (int i=0; i<nrTextures; i++)
			DrawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

		// Create attached textures
		textures = new Texture[nrTextures];
		for (int i=0; i<nrTextures; i++)
			textures[i].CreateFrameBufferTexture(width, height, i);

		glDrawBuffers(nrTextures, DrawBuffers);

	} else {
		//glDrawBuffer(GL_NONE);	// Disable writes from the color buffer
		//glReadBuffer(GL_NONE);	// Disable reads from the color buffer
	}

	// Create depth texture
	depthTexture->CreateDepthBufferTexture(width, height);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		cout << "FRAMEBUFFER NOT COMPLETE" << endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	CheckOpenGLError();
}

void FrameBuffer::Bind(bool clearBuffer) const {
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glViewport(0, 0, width, height);
	if (clearBuffer)
		FrameBuffer::Clear();
}

void FrameBuffer::SendResolution(Shader *shader) const {
	glUniform2i(shader->loc_resolution, width, height);
}

glm::ivec2 FrameBuffer::GetResolution() const {
	return glm::ivec2(width, height);
}

void FrameBuffer::BindTexture(int textureID, GLenum TextureUnit) const {
	textures[textureID].Bind(TextureUnit);
}

void FrameBuffer::BindDepthTexture(GLenum TextureUnit) const {
	depthTexture->Bind(TextureUnit);
}

void FrameBuffer::BindAllTextures() const{
	for (unsigned int i=0; i<nrTextures; i++) {
		textures[i].Bind(GL_TEXTURE0 + i);
	}
}

void FrameBuffer::Unbind() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, Engine::Window->resolution.x, Engine::Window->resolution.y);
}

void FrameBuffer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
