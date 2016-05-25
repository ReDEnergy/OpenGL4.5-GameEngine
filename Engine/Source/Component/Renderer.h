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
		bool IsTransparent() const;
		unsigned char GetOpacity() const;
		bool IsRendered() const;

		void Use() const;
		void SetCulling(OpenGL::CULL face);
		void SetCastShadow(bool value);
		void SetRenderingLayer(RenderingLayer layer);
		RenderingLayer GetRenderingLayer();

		// Set oppacity value should be between 0 and 100
		void SetOpacity(unsigned char value);

		void SetIsRendered(bool value);
		void SetUseAlphaChannel(bool value);

	private:
		OpenGL::CULL culling;
		RenderingLayer layer;
		bool alphaChannel;
		bool castSadows;
		unsigned char opacity;
		bool isRendered;
};

