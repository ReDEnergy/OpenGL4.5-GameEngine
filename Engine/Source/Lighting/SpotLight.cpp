//#include <pch.h>
#include "SpotLight.h"

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

SpotLight::SpotLight()
	: GameObject("stop-light")
{
	Init();
}

SpotLight::~SpotLight()
{
}

void SpotLight::Init()
{
	FBO = new FrameBuffer();
	FBO->Generate(2048, 2048, 1);

	//forward = glm::vec3(0.0f, -1.0f, 0.0f);
	//up = glm::cross(right, forward);

	bulbSize = glm::vec3(5);
	diffuseColor = glm::vec3(0.90f, 0.63f, 0.13f);

	lightProjections.resize(splits);
	lightViews.resize(splits);
}

void SpotLight::SplitFrustum(unsigned int splits)
{
	lightProjections.resize(splits);
	lightViews.resize(splits);
	Camera::SplitFrustum(splits);
}

void SpotLight::SetDebugView(bool value)
{
	GameObject::SetDebugView(value);
}

void SpotLight::Update()
{
	Camera::Update();
	if (splits > 1) {
		for (unsigned int i = 0; i < splits; i++) {
			lightProjections[i] = glm::perspective(FOV, aspectRatio, splitDistances[i], splitDistances[i + 1]);
			lightViews[i] = View;
		}
	}
};

void SpotLight::CastShadows()
{
	// Render pass
	FBO->Bind(false);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// glClear(GL_DEPTH_BUFFER_BIT);

	Shader *CSHM = Manager::Shader->GetShader("VSM");
	CSHM->Use();

	BindViewMatrix(CSHM->loc_view_matrix);
	BindProjectionMatrix(CSHM->loc_projection_matrix);

	for (auto *obj : Manager::Scene->GetActiveObjects()) {
		if (obj->renderer->CastShadow())
			obj->Render(CSHM);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	FrameBuffer::Unbind();
}

void SpotLight::Render(const Shader * shader) const
{
	GameObject::Render(shader);
}

void SpotLight::RenderDebug(const Shader *shader) const {
	Camera::RenderDebug(shader);
}

void SpotLight::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void SpotLight::BakeShadows(const FrameBuffer * const sceneBuffer) const
{
	int WORKGROUP_SIZE = 32;

	Shader *sha = Manager::GetShader()->GetShader("ShadowMap");
	sha->Use();

	sceneBuffer->BindDepthTexture(GL_TEXTURE0);
	Manager::GetScene()->GetActiveCamera()->BindProjectionDistances(sha);

	glBindImageTexture(0, sceneBuffer->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, sceneBuffer->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	BindForUse(sha);
	glUniform1i(sha->loc_shadowID, 100);

	glDispatchCompute(GLuint(UPPER_BOUND(sceneBuffer->GetResolution().x, WORKGROUP_SIZE)), GLuint(UPPER_BOUND(sceneBuffer->GetResolution().y, WORKGROUP_SIZE)), 1);
	glMemoryBarrier(GL_ALL_BARRIER_BITS);
}

void SpotLight::BindForUse(const Shader *shader) const
{
	BindViewMatrix(shader->loc_view_matrix);
	BindProjectionMatrix(shader->loc_projection_matrix);
	BindProjectionDistances(shader);

	FBO->BindTexture(0, GL_TEXTURE0);
	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);
}
