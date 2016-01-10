//#include <pch.h>
#include "DirectionalLight.h"

#include <include/math.h>

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

void DirectionalLight::Init()
{
	FBO = new FrameBuffer();
	FBO->Generate(2048, 2048, 1);

	transform->SetWorldRotation(glm::vec3(45, 0, 0));

	bulbSize = glm::vec3(5);
	diffuseColor = glm::vec3(0.90f, 0.63f, 0.13f);

	int splits = 5;
	lightViews.resize(splits);
	lightProjections.resize(splits);
}

void DirectionalLight::Update() {
	Camera::Update();
}
void DirectionalLight::SetCamera(const Camera* camera)
{
	viewCamera = camera;
}

void DirectionalLight::SetDebugView(bool value)
{
	GameObject::SetDebugView(value);
}


void DirectionalLight::CastShadows()
{
	// Render pass
	FBO->Bind(false);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	///////////////////////////////////////////////////////////////////////////

	Shader *CSHM = Manager::Shader->GetShader("CSM");
	CSHM->Use();

	for (unsigned int i = 0; i < viewCamera->splits; i++) {

		glColorMask(i == 0, i == 1, i == 2, i == 3);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniform1i(CSHM->CSM_cascadeID, i);
		glUniformMatrix4fv(CSHM->loc_view_matrix, 1, false, glm::value_ptr(lightViews[i]));
		glUniformMatrix4fv(CSHM->loc_projection_matrix, 1, false, glm::value_ptr(lightProjections[i]));

		for (auto *obj : Manager::Scene->GetFrustrumObjects()) {
			if (obj->renderer->CastShadow())
				obj->Render(CSHM);
		}
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	FrameBuffer::Unbind();
}

void DirectionalLight::Render(const Shader * shader) const
{
	GameObject::Render(shader);
}

void DirectionalLight::RenderDebug(const Shader *shader) const
{
	Light::RenderDebug(shader);
}

void DirectionalLight::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void DirectionalLight::BakeShadows(const FrameBuffer* const sceneBuffer) const
{
	int WORKGROUP_SIZE = 32;

	Shader *sha = Manager::GetShader()->GetShader("CSMShadowMap");
	sha->Use();

	sceneBuffer->BindDepthTexture(GL_TEXTURE0);
	viewCamera->BindProjectionDistances(sha);

	glBindImageTexture(0, sceneBuffer->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, sceneBuffer->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	BindForUse(sha);
	glUniform1i(sha->loc_shadowID, 0);

	glDispatchCompute(GLuint(UPPER_BOUND(sceneBuffer->GetResolution().x, WORKGROUP_SIZE)), GLuint(UPPER_BOUND(sceneBuffer->GetResolution().y, WORKGROUP_SIZE)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void DirectionalLight::BindForUse(const Shader *shader) const
{
	glUniform1fv(shader->CSM_SplitDistance, viewCamera->splits, &viewCamera->splitDistances[1]);
	glUniformMatrix4fv(shader->CSM_LightView, viewCamera->splits, false, glm::value_ptr(lightViews[0]));
	glUniformMatrix4fv(shader->CSM_LightProjection, viewCamera->splits, false, glm::value_ptr(lightProjections[0]));

	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);

	FBO->BindTexture(0, GL_TEXTURE1);
	FBO->BindDepthTexture(GL_TEXTURE2);
}