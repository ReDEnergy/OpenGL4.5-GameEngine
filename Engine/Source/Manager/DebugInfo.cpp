//#include <pch.h>
#include "DebugInfo.h"

#include <cstdio>
#include <cstring>
#include <string>

#include <include/util.h>

#include <Component/AABB.h>
#include <Core/Engine.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <GPU/FrameBuffer.h>
#include <GPU/Shader.h>

#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>

#include <Utils/GPU.h>

DebugInfo::DebugInfo() {
	debugMessages = false;
	debugView = false;
}

DebugInfo::~DebugInfo() {
	SAFE_FREE(FBO);
}

void DebugInfo::Init() {
	FBO = new FrameBuffer();
	FBO->Generate(Engine::Window->resolution.x, Engine::Window->resolution.y, 1);
}

void DebugInfo::InitManager(const char *info) {
	if (debugMessages == false) return;
	int len = strlen(info);
	printf("\n----------------------------------------------------------------------\n");
	printf("Load %s\n\n", info);
}

void DebugInfo::Add(GameObject *obj) {
	objects.push_back(obj);
}

void DebugInfo::Remove(GameObject *obj) {
	objects.remove(obj);
}

void DebugInfo::BindForRendering(const Camera *camera) const {
	FBO->Bind();
	Shader *S = Manager::Shader->GetShader("simple");
	S->Use();
	camera->BindViewMatrix(S->loc_view_matrix);
	camera->BindProjectionMatrix(S->loc_projection_matrix);
}

void DebugInfo::Render(const Camera *camera) const {
	FBO->Bind(false);
	Shader *S = Manager::Shader->GetShader("simple");
	S->Use();

	camera->BindViewMatrix(S->loc_view_matrix);
	camera->BindProjectionMatrix(S->loc_projection_matrix);

	for (auto obj: objects) {
		obj->RenderDebug(S);
	}

	{
		Manager::RenderSys->Set(RenderState::WIREFRAME, true);
		glLineWidth(2);
		for (auto *obj : Manager::Scene->frustumObjects) {
			if (obj->aabb) {
				obj->aabb->Render(S);
			}
		}
		glLineWidth(1);
		Manager::RenderSys->Revert(RenderState::WIREFRAME);
	}

	{
		// Draw World AXIS
		float size = 100;
		glLineWidth(4);
		glUniformMatrix4fv(S->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniform4f(S->loc_debug_color, 0.92f, 0.15f, 0.15f, 1.0f);
		UtilsGPU::DrawLine(glm::vec3(0, 0, 0), glm::vec3(size, 0, 0));
		glUniform4f(S->loc_debug_color, 0.19f, 0.92f, 0.15f, 1.0f);
		UtilsGPU::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, size, 0));
		glUniform4f(S->loc_debug_color, 0.15f, 0.59f, 0.92f, 1.0f);
		UtilsGPU::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, size));
		glLineWidth(1);
	}
	FrameBuffer::Unbind();
}