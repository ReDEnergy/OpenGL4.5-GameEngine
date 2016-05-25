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
#include <Rendering/DirectOpenGL.h>

#include <Utils/OpenGL.h>

DebugInfo::DebugInfo()
{
	debugView = false;
	bboxMode = BBOX_MODE::LIGHT_SPACE;
}

DebugInfo::~DebugInfo() {
	SAFE_FREE(FBO);
}

void DebugInfo::Init()
{
	FBO = new FrameBuffer();
	FBO->Generate(Engine::Window->props.resolution.x, Engine::Window->props.resolution.y, 1);
	Manager::TextureDBG->SetChannel(2, FBO);
}

void DebugInfo::InitManager(const char *info)
{
#ifdef _DEBUG
	printf("MANAGER:: Loaded %s\n\n", info);
#endif
}

void DebugInfo::Add(GameObject *obj)
{
	objects.push_back(obj);
}

void DebugInfo::Remove(GameObject *obj)
{
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

void DebugInfo::SetBoundingBoxMode(BBOX_MODE mode)
{
	bboxMode = mode;

	for (auto &obj : objects) {
		if (obj->aabb)
			obj->aabb->ComputeLocal();
	}
}

bool DebugInfo::GetActiveState() const
{
	return debugView;
}

DebugInfo::BBOX_MODE DebugInfo::GetBoundingBoxMode() const
{
	return bboxMode;
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

	// TODO: Instantiated rendering
	// - use a simplified box not one with 12 triangles - should be only 6 quads

	GL_Utils::SetColorUnit(S->loc_debug_color, 1, 1, 1);

	Manager::RenderSys->Set(RenderState::WIREFRAME, true);

	if (bboxMode == BBOX_MODE::OBJECT_SAPCE) {
		for (auto &obj : objects)
		{
			obj->aabb->RenderObjectBoundingBox(S);
		}
	} else {
		for (auto &obj : objects)
		{
			obj->aabb->Render(S);
		}
	}

	Manager::RenderSys->Revert(RenderState::WIREFRAME);

	// TODO: Instantiated rendering
	auto DirectGL = Manager::GetDirectGL();
	DirectGL->SetLineWidth(3);
	for (auto &obj: objects)
	{
		DirectGL->DrawStandardAxis(obj->transform, S);
	}

	Transform x;
	x.SetScale(glm::vec3(100));
	DirectGL->DrawStandardAxis(&x, S);


	for (auto &obj : objects)
	{
		obj->RenderDebug(S);
	}
}