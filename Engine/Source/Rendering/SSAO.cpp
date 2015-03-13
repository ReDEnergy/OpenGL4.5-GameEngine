//#include <pch.h>
#include "SSAO.h"

#include <include/math.h>

#include <Core/Camera/Camera.h>
#include <GPU/Texture.h>
#include <GPU/Shader.h>

#include <Manager/Manager.h>
#include <Manager/TextureManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/ResourceManager.h>


SSAO::SSAO()
{
	radius = 0.05f;
	kernelSize = 16;

	float scale;
	for (int i = 0; i < kernelSize; ++i) {
		kernel[i] = glm::vec3(
			(float(rand()) / RAND_MAX) * 2 - 1.0f, 
			(float(rand()) / RAND_MAX) * 2 - 1.0f, 
			float(rand()) / RAND_MAX);
		kernel[i] = glm::normalize(kernel[i]);
		kernel[i] *= float(rand()) / RAND_MAX;

		scale = float(i) / kernelSize;
		scale = lerp(0.1f, 1.0f, scale * scale);
		kernel[i] *= scale;
	}

	ScreenQuad = Manager::Resource->GetGameObject("render-quad");
	RandomNoise1 = Manager::Texture->GetTexture("random.jpg");
	RandomNoise2 = Manager::Texture->GetTexture("noise.png");

	computeTexture = new Texture();
}

SSAO::~SSAO() {
}

void SSAO::Init(int width, int height)
{
	ssaoFBO.Generate(width, height, 1);
	computeTexture->Create2DTextureFloat(NULL, width, height, 4);
}

void SSAO::Update(const FrameBuffer *FBO, const Camera *camera) const
{
	ssaoFBO.Bind();
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	Shader *ssao = Manager::Shader->GetShader("ssao");
	ssao->Use();
	ssao->BindTexturesUnits();

	ssaoFBO.SendResolution(ssao);
	camera->BindViewMatrix(ssao->loc_view_matrix);
	camera->BindProjectionMatrix(ssao->loc_projection_matrix);
	camera->BindProjectionDistances(ssao);

	glUniform1f(ssao->loc_u_rad, radius);
	glUniform1i(ssao->loc_kernel_size, kernelSize);
	glUniform3fv(ssao->loc_kernel, kernelSize * 3, glm::value_ptr(kernel[0]));

	FBO->BindTexture(3, GL_TEXTURE0);
	FBO->BindTexture(4, GL_TEXTURE1);
	FBO->BindDepthTexture(GL_TEXTURE2);
	RandomNoise1->Bind(GL_TEXTURE3);
	RandomNoise2->Bind(GL_TEXTURE4);

	ScreenQuad->Render(ssao);

	// Finish TASK
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	FrameBuffer::Unbind();

	// -- COMPUTE SHADER
	int WORK_GROUP_SIZE = 16;
	auto res = ssaoFBO.GetResolution();

	Shader *S = Manager::Shader->GetShader("compute");
	S->Use();

	// First Pass
	ssaoFBO.BindTexture(0, GL_TEXTURE0);
	
	glBindImageTexture(1, computeTexture->GetTextureID(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glDispatchCompute(GLuint(UPPER_BOUND(res.x, WORK_GROUP_SIZE)), GLuint(UPPER_BOUND(res.y, WORK_GROUP_SIZE)), 1);

	glFinish();
	glMemoryBarrier(GL_ALL_BARRIER_BITS);

}

void SSAO::BindTexture(GLenum TextureUnit) {
	computeTexture->Bind(TextureUnit);
}