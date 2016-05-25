#include "Renderer.h"

#include <Manager/Manager.h>
#include <Manager/RenderingSystem.h>

Renderer::Renderer()
{
	alphaChannel = false;
	isRendered = true;
	castSadows = true;
	opacity = 100;
	culling = OpenGL::CULL::BACK;
	layer = RenderingLayer::DEFAULT;
}

Renderer::~Renderer()
{
}

bool Renderer::CastShadow() const
{
	return castSadows;
}

bool Renderer::IsTransparent() const
{
	return (alphaChannel || (opacity < 100));
}

unsigned char Renderer::GetOpacity() const
{
	return opacity;
}

bool Renderer::IsRendered() const
{
	return isRendered;
}

RenderingLayer Renderer::GetRenderingLayer()
{
	return layer;
}

void Renderer::SetRenderingLayer(RenderingLayer layer)
{
	this->layer = layer;
}

void Renderer::SetOpacity(unsigned char value)
{
	opacity = value;
}

void Renderer::SetIsRendered(bool value)
{
	isRendered = value;
}

void Renderer::SetUseAlphaChannel(bool value)
{
	alphaChannel = value;
}

void Renderer::Use() const
{
	Manager::RenderSys->CullFace(culling);
}

void Renderer::SetCulling(OpenGL::CULL face)
{
	culling = face;
}

void Renderer::SetCastShadow(bool value) {
	castSadows = value;
}
