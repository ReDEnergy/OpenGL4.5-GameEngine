#include "RenderingSystem.h"

#include <include/gl.h>

void RenderingSystem::Init()
{
	debugParam = 0;
	states = new bool[10]();
	prevStates = new bool[10]();
	Set(RenderState::POST_PROCESS, true);
	Set(RenderState::SHADOW, true);
	Set(RenderState::SS_AO, false);
	Set(RenderState::HIDE_POINTER, true);
	Set(RenderState::CLIP_POINTER, true);

	culling = OpenGL::CULL::NONE;
	globalCulling = false;

	// glLineWidth
	lineWidth = 1;

	//glEnable(GL_LINE_SMOOTH);
}

bool RenderingSystem::Is(RenderState STATE)
{
	return states[STATE] == true;
}

void RenderingSystem::Set(RenderState STATE, bool value)
{
	SavePreviousState(STATE);
	states[STATE] = value;
#ifndef OPENGL_ES
	if (STATE == RenderState::WIREFRAME)
		value ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
	UpdateGlobalState();
}

void RenderingSystem::Revert(RenderState STATE)
{
	bool value = prevStates[STATE];
	Set(STATE, value);
}

bool RenderingSystem::Toggle(RenderState STATE)
{
	SavePreviousState(STATE);
	states[STATE] = !states[STATE];
	UpdateGlobalState();
	return states[STATE];
}

void RenderingSystem::SetGlobalCulling(OpenGL::CULL faces)
{
	globalCulling = false;
	CullFace(faces);
	globalCulling = true;
}

void RenderingSystem::DisableGlobalCulling()
{
	globalCulling = false;
	RevertPreviousCulling();
}

void RenderingSystem::CullFace(OpenGL::CULL state)
{
	if (globalCulling || culling == state)
		return;

	if (state == OpenGL::CULL::NONE) {
		glDisable(GL_CULL_FACE);
	}
	else {
		if (culling == OpenGL::CULL::NONE)
			glEnable(GL_CULL_FACE);

		if (state == OpenGL::CULL::FRONT)
			glCullFace(GL_FRONT);
		else if (state == OpenGL::CULL::BACK)
			glCullFace(GL_BACK);
		else
			glCullFace(GL_FRONT_AND_BACK);
	}

	prevCulling = culling;
	culling = state;
	CheckOpenGLError();
}

void RenderingSystem::RevertPreviousCulling()
{
	CullFace(prevCulling);
}

void RenderingSystem::SetLineWidth(float width)
{
	if (lineWidth != width)
	{
		glLineWidth(width);
		lineWidth = width;
	}
}

void RenderingSystem::UpdateGlobalState()
{
	states[RenderState::FORWARD] = false;
	if (states[RenderState::WIREFRAME] || !states[RenderState::POST_PROCESS])
	{
		SavePreviousState(RenderState::FORWARD);
		states[RenderState::FORWARD] = true;
	}
}

void RenderingSystem::SavePreviousState(RenderState STATE)
{
	prevStates[STATE] = states[STATE];
}