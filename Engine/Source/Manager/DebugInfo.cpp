//#include <pch.h>
#include "DebugInfo.h"

#include <cstdio>
#include <cstring>
#include <string>

#include <include/utils.h>
#include <include/gl_utils.h>

#include <Component/AABB.h>
#include <Component/Transform/Transform.h>

#include <Core/Engine.h>
#include <Core/WindowObject.h>
#include <Core/Camera/Camera.h>
#include <Core/GameObject.h>
#include <GPU/FrameBuffer.h>
#include <GPU/Shader.h>

#include <Manager/SceneManager.h>
#include <Manager/ShaderManager.h>
#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>
#include <Debugging/TextureDebugger.h>

#include <Utils/OpenGL.h>

DebugInfo::DebugInfo() {
	debugView = false;
}

DebugInfo::~DebugInfo() {
	SAFE_FREE(FBO);
}

void DebugInfo::Init()
{
	FBO = new FrameBuffer();
	FBO->Generate(Engine::Window->resolution.x, Engine::Window->resolution.y, 1);
	Manager::TextureDBG->SetChannel(2, FBO);
}

void DebugInfo::InitManager(const char *info)
{
#ifdef _DEBUG
	printf("MANAGER:: Loaded %s\n\n", info);
#endif
}

void DebugInfo::Add(GameObject *obj) {
	objects.push_back(obj);
}

void DebugInfo::Remove(GameObject *obj) {
	objects.remove(obj);
}

void DebugInfo::Update(const Camera * camera)
{
	if (!debugView) return;
	
	Render(camera);
}

bool DebugInfo::Toggle()
{
	debugView = !debugView;
	return debugView;
}

bool DebugInfo::GetActiveState() const
{
	return debugView;
}

void DebugInfo::Render(const Camera *camera) const
{
	Shader *S = Manager::Shader->GetShader("simple");
	S->Use();

	camera->BindViewMatrix(S->loc_view_matrix);
	camera->BindProjectionMatrix(S->loc_projection_matrix);

	FBO->Bind();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glLineWidth(2);
	for (auto obj: objects)
	{
		obj->RenderDebug(S);
	}

	{
		// Draw World AXIS
		float size = 100;
		glLineWidth(4);
		glUniformMatrix4fv(S->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
		glUniform4f(S->loc_debug_color, 0.92f, 0.15f, 0.15f, 1.0f);
		OpenGL::DrawLine(glm::vec3(0, 0, 0), glm::vec3(size, 0, 0));
		glUniform4f(S->loc_debug_color, 0.19f, 0.92f, 0.15f, 1.0f);
		OpenGL::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, size, 0));
		glUniform4f(S->loc_debug_color, 0.15f, 0.59f, 0.92f, 1.0f);
		OpenGL::DrawLine(glm::vec3(0, 0, 0), glm::vec3(0, 0, size));
		glLineWidth(1);
	}
	FrameBuffer::Unbind();
}