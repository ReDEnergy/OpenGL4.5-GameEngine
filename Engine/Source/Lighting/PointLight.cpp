#include "PointLight.h"

#include <include/gl.h>
#include <include/glm_utils.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/Transform/Transform.h>
#include <Component/Renderer.h>

#include <GPU/Shader.h>
#include <GPU/Texture.h>
#include <GPU/FrameBuffer.h>

#include <Manager/Manager.h>
#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>

struct CameraDirection
{
	GLenum cubeMapFace;
	glm::vec3 target;
	glm::vec3 up;
};

CameraDirection cameraDirections[6] =
{
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_X, glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_X, glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, 1.0f,  0.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Y, glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f, 0.0f,  1.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f, 0.0f, -1.0f) },
	{ GL_TEXTURE_CUBE_MAP_POSITIVE_Z, glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, 1.0f,  0.0f) },
	{ GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, 1.0f,  0.0f) }
};


PointLight::PointLight()
	: GameObject("point-light")
{
	effectRadius = 10;
}

PointLight::PointLight(const GameObject &obj)
	: GameObject(obj)
{
	effectRadius = 10;
}

void PointLight::RenderDeferred(Shader *shader)
{
	auto pos = transform->GetWorldPosition();
	glUniform1f(shader->loc_light_radius, effectRadius / 2);
	glUniform3f(shader->loc_light_color, diffuseColor.x, diffuseColor.y, diffuseColor.z);
	glUniform3f(shader->loc_light_pos, pos.x, pos.y, pos.z);

	transform->SetScale(glm::vec3(effectRadius));
	GameObject::Render(shader);
}

void PointLight::CastShadows()
{

	// Render pass
	FBO->Bind();

	Shader *VSM = Manager::Shader->GetShader("VSM");
	VSM->Use();

	for (unsigned int i = 0; i < 6; i++) {
		cubeTexture->BindToTarget(cameraDirections[i].cubeMapFace);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// TODO 
		camera->transform->SetWorldPosition(transform->GetWorldPosition());
		camera->transform->SetWorldRotation(cameraDirections[i].target * 90.0f);
		//camera->Set(transform->GetWorldPosition(), cameraDirections[i].target, cameraDirections[i].up);
		camera->BindViewMatrix(VSM->loc_view_matrix);
		camera->BindProjectionMatrix(VSM->loc_projection_matrix);

		for (auto *obj : Manager::Scene->GetSceneObjects()) {
			if (obj->renderer->CastShadow())
				obj->Render(VSM);
		}
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	FrameBuffer::Unbind();
}

void PointLight::InitCaster()
{
	FBO = new FrameBuffer();
	FBO->Generate(1024, 1024, 0);
	cubeTexture = new Texture();
	cubeTexture->CreateCubeTexture(NULL, 1024, 1024, 4);
	camera = new Camera();
	camera->SetPerspective(90, 1, 0.1f, 50);
}

void PointLight::BindForUse(const Shader *shader) const
{
	auto pos = transform->GetWorldPosition();
	camera->BindViewMatrix(shader->loc_view_matrix);
	camera->BindProjectionMatrix(shader->loc_projection_matrix);
	camera->BindProjectionDistances(shader);
	glUniform3f(shader->loc_light_pos, pos.x, pos.y, pos.z);
	BindTexture(GL_TEXTURE2);

	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);
}

void PointLight::BindTexture(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture->GetTextureID());
}

void PointLight::SetArea(float radius)
{
	effectRadius = radius;
}

void PointLight::SetDebugView(bool value)
{
	GameObject::SetDebugView(value);
}
