//#include <pch.h>
#include "SpotLight.h"

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

void SpotLight::Init() {
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

void SpotLight::Update() {
	Camera::Update();
	if (splits > 1) {
		for (unsigned int i = 0; i < splits; i++) {
			lightProjections[i] = glm::perspective(FOV, aspectRatio, splitDistances[i], splitDistances[i + 1]);
			lightViews[i] = View;
		}
	}
};

void SpotLight::CastShadows() {
	// Render pass
	FBO->Bind(false);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	// glClear(GL_DEPTH_BUFFER_BIT);

	Shader *CSHM = Manager::Shader->GetShader("VSM");
	CSHM->Use();

	BindViewMatrix(CSHM->loc_view_matrix);
	BindProjectionMatrix(CSHM->loc_projection_matrix);

	for (auto *obj : Manager::Scene->activeObjects) {
		if (obj->renderer->CastShadow())
			obj->Render(CSHM);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	FrameBuffer::Unbind();
}

void SpotLight::RenderDebug(const Shader *shader) const {
	Camera::RenderDebug(shader);
}

void SpotLight::BindForUse(const Shader *shader) const
{
	BindViewMatrix(shader->loc_view_matrix);
	BindProjectionMatrix(shader->loc_projection_matrix);
	BindProjectionDistances(shader);

	FBO->BindTexture(0, GL_TEXTURE1);
	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);
}
