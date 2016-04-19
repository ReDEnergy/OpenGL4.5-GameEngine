#pragma once

#include <include/dll_export.h>
#include <Rendering/Rendering.h>

/*
 * Class Renderer
 */

enum class RenderingLayer
{
	DEFAULT,
	ON_TOP
};

class DLLExport Renderer
{
	public:
		Renderer();
		virtual ~Renderer();

		bool CastShadow() const;
		RenderingLayer GetRenderingLayer();
		bool IsTransparent() const;
		float GetOpacity() const;
		bool IsRendered() const;

		void Use() const;
		void SetCulling(OpenGL::CULL face);
		void SetCastShadow(bool value);
		void SetRenderingLayer(RenderingLayer layer);
		void SetOpacity(float value);
		void SetIsRendered(bool value);
		void SetUseAlphaChannel(bool value);

	private:
		OpenGL::CULL culling;
		RenderingLayer layer;
		bool alphaChannel;
		bool castSadows;
		float opacity;
		bool isRendered;
};

