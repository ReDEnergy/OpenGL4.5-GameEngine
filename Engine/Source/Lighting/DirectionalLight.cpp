//#include <pch.h>
#include "DirectionalLight.h"

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/AABB.h>
#include <Component/Renderer.h>
#include <Component/Transform/Transform.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Shader.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>

using namespace std;

DirectionalLight::DirectionalLight()
	: GameObject("dir-light")
{
	distanceTo = 100.0f;
	Camera::Init();
	Init();
}

DirectionalLight::~DirectionalLight() 
{
}

void DirectionalLight::Init() {
	FBO = new FrameBuffer();
	FBO->Generate(2048, 2048, 1);

	transform->SetWorldRotation(glm::vec3(45, 0, 0));

	// TODO set forward
	//forward = glm::vec3(0.0f, -1.0f, 0.0f);
	//up = glm::cross(right, forward);
	bulbSize = glm::vec3(5);
	diffuseColor = glm::vec3(0.90f, 0.63f, 0.13f);

	int splits = 5;
	lightViews.resize(splits);
	lightProjections.resize(splits);
}

void DirectionalLight::Update() {
	Camera::Update();
};

void DirectionalLight::CastShadows(const Camera *camera) {
	// Render pass
	FBO->Bind(false);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);


	///////////////////////////////////////////////////////////////////////////
	// Clear channels that must be updated using a Compute Shader
	//{
	//	const int CLEAR = 0;
	//	const int KEEP = 1;

	//	Shader *CSClear = Manager::Shader->GetShader("CSMClear");
	//	CSClear->Use();

	//	glBindImageTexture(0, FBO->textures[0].GetTextureID(), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	//	glDispatchCompute(GLuint(UPPER_BOUND(FBO->GetResolution().x, 16)), GLuint(UPPER_BOUND(2048, 16)), 1);
	//	glm::BindUniform4f(CSClear->loc_channel_mask, glm::vec4(1, 1, DIV(updateCount, 2), DIV(updateCount, 3)));

	//	glFinish();
	//	glMemoryBarrier(GL_ALL_BARRIER_BITS);
	//}

	//if (i > 1 && i != 4) {
	//	if (DIV(updateCount, i+1) != 0)
	//		continue;
	//}

	///////////////////////////////////////////////////////////////////////////

	Shader *CSHM = Manager::Shader->GetShader("CSM");
	CSHM->Use();

	for (unsigned int i = 0; i < camera->splits; i++) {

		glColorMask(i == 0, i == 1, i == 2, i == 3);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniform1i(CSHM->CSM_cascadeID, i);
		glUniformMatrix4fv(CSHM->loc_view_matrix, 1, false, glm::value_ptr(lightViews[i]));
		glUniformMatrix4fv(CSHM->loc_projection_matrix, 1, false, glm::value_ptr(lightProjections[i]));

		for (auto *obj : Manager::Scene->frustumObjects) {
			if (obj->renderer->CastShadow())
				obj->Render(CSHM);
		}
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	//glCullFace(GL_BACK);
	//glDisable(GL_CULL_FACE);
	FrameBuffer::Unbind();
}

void DirectionalLight::RenderDebug(const Shader *shader) const {
	Light::RenderDebug(shader);
}

void DirectionalLight::BindForUse(const Shader *shader, Camera *camera) const
{
	glUniform1fv(shader->CSM_SplitDistance, camera->splits, &camera->splitDistances[1]);
	glUniformMatrix4fv(shader->CSM_LightView, camera->splits, false, glm::value_ptr(lightViews[0]));
	glUniformMatrix4fv(shader->CSM_LightProjection, camera->splits, false, glm::value_ptr(lightProjections[0]));

	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);

	FBO->BindTexture(0, GL_TEXTURE1);
	FBO->BindDepthTexture(GL_TEXTURE2);
}
