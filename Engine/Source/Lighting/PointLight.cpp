//#include <pch.h>
#include "PointLight.h"

#include <include/glm_utils.h>

#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <Component/Transform.h>
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


PointLight::PointLight() {
}

void PointLight::RenderDeferred(Shader *shader) {
	glUniform1f(shader->loc_light_radius, transform->scale.x / 2.0f);
	glm::BindUniform3f(shader->loc_light_color, diffuseColor);
	glm::BindUniform3f(shader->loc_light_pos, transform->position);
	Render(shader);
}

void PointLight::CastShadows() {
	
	// Render pass
	FBO->Bind();

	Shader *CSHM = Manager::Shader->GetShader("VSM");
	CSHM->Use();

	for (unsigned int i = 0; i < 6; i++) {
		cubeTexture->BindForWriting(cameraDirections[i].cubeMapFace);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera->Set(transform->position, cameraDirections[i].target, cameraDirections[i].up);
		camera->BindViewMatrix(CSHM->loc_view_matrix);
		camera->BindProjectionMatrix(CSHM->loc_projection_matrix);

		for (auto *obj : Manager::Scene->activeObjects) {
			if (obj->renderer->CastShadow())
				obj->Render(CSHM);
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
	camera->BindViewMatrix(shader->loc_view_matrix);
	camera->BindProjectionMatrix(shader->loc_projection_matrix);
	camera->BindProjectionDistances(shader);
	glm::BindUniform3f(shader->loc_light_pos, transform->position);
	BindTexture(GL_TEXTURE2);

	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(shader->loc_shadow_texel_size, 1.0f / rez.x, 1.0f / rez.y);
}

void PointLight::BindTexture(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexture->GetTextureID());
}
