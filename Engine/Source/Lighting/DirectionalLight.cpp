#include "DirectionalLight.h"

#include <include/gl.h>
#include <include/math.h>

#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Core/WindowObject.h>
#include <Component/AABB.h>
#include <Component/Renderer.h>
#include <Component/Transform/Transform.h>

#include <GPU/FrameBuffer.h>
#include <GPU/Shader.h>

#include <Utils/OpenGL.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Debugging/TextureDebugger.h>

#include <Lighting/FrustumSplit.h>

using namespace std;

DirectionalLight::DirectionalLight()
	: GameObject("dir-light")
{
	csmFrustum = nullptr;
	Init();
	Camera::Init();
}

DirectionalLight::~DirectionalLight() 
{
}

void DirectionalLight::Init()
{

	FBO = new FrameBuffer();
	FBO->Generate(2048, 2048, 1);

	bulbSize = glm::vec3(5);
	diffuseColor = glm::vec3(0.90f, 0.63f, 0.13f);
}

void DirectionalLight::Update()
{
	Camera::Update();
	if (csmFrustum && viewCamera->transform->GetMotionState())
		csmFrustum->Update(this);
}

void DirectionalLight::SetCamera(const Camera* camera)
{
	viewCamera = camera;
	csmFrustum = new FrustumSplit();
	csmFrustum->SetCamera(viewCamera);
	csmFrustum->Update(this);
}

void DirectionalLight::SetDebugView(bool value)
{
	GameObject::SetDebugView(value);
}

const FrameBuffer * DirectionalLight::GetTextureBuffer() const
{
	return FBO;
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

	for (unsigned int i = 0; i < 5; i++) {

		glColorMask(i == 0, i == 1, i == 2, i == 3);
		glClear(GL_DEPTH_BUFFER_BIT);

		glUniform1i(CSHM->CSM_cascadeID, i);
		csmFrustum->BindViewProjection(CSHM, i);

		for (auto *obj : Manager::Scene->GetFrustrumObjects()) {
			if (obj->renderer->CastShadow())
				obj->Render(CSHM);
		}
	}

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
	csmFrustum->RenderDebug(shader);
}

void DirectionalLight::RenderForPicking(const Shader * shader) const
{
	GameObject::RenderForPicking(shader);
}

void DirectionalLight::BakeShadows(const FrameBuffer* const sceneBuffer) const
{
	Shader *sha = Manager::GetShader()->GetShader("CSMShadowMap");
	sha->Use();

	sceneBuffer->BindDepthTexture(GL_TEXTURE0);
	viewCamera->BindProjectionDistances(sha);

	glBindImageTexture(0, sceneBuffer->GetTextureID(0), 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
	glBindImageTexture(1, sceneBuffer->GetTextureID(1), 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA32F);

	BindForUse(sha);
	glUniform1i(sha->loc_shadowID, 0);

	auto resolution = sceneBuffer->GetResolution();
	OpenGL::DispatchCompute(resolution.x, resolution.y, 1, 32);
}

void DirectionalLight::BindForUse(const Shader *shader) const
{
	csmFrustum->BindForUse(shader);

	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);

	FBO->BindTexture(0, GL_TEXTURE1);
	FBO->BindDepthTexture(GL_TEXTURE2);
}