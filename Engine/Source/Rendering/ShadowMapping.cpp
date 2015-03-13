#include "ShadowMapping.h"
#include <include/gl.h>

#include <Core/GameObject.h>
#include <Component/Renderer.h>
#include <GPU/FrameBuffer.h>
#include <GPU/Shader.h>
#include <Lighting/DirectionalLight.h>
#include <Manager/Manager.h>
#include <Manager/ShaderManager.h>
#include <Manager/SceneManager.h>

ShadowMapping::ShadowMapping() {
	FBO = new FrameBuffer();
	FBO->Generate(4096, 4096, 0);
}

ShadowMapping::~ShadowMapping() {
}

void ShadowMapping::Init() {
}

void ShadowMapping::Update(DirectionalLight *lightSource) {

	FBO->Bind();
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glCullFace(GL_FRONT);

	Shader *SHM = Manager::Shader->GetShader("shadow");
	SHM->Use();

	lightSource->BindViewMatrix(SHM->loc_view_matrix);
	lightSource->BindProjectionMatrix(SHM->loc_projection_matrix);

	for (auto *obj: Manager::Scene->frustumObjects) {
		if (obj->renderer->CastShadow())
			obj->Render(SHM);
	}

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glCullFace(GL_BACK);
	FrameBuffer::Unbind();
}

void ShadowMapping::BindTexelSize(GLint location)
{
	glm::ivec2 rez = FBO->GetResolution();
	glUniform2f(location, 1.0f/rez.x, 1.0f/rez.y);
}
