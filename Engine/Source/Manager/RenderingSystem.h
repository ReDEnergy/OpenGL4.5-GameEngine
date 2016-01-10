#pragma once

#include <include/dll_export.h>
#include <Rendering/Rendering.h>

using namespace std;

class DLLExport RenderingSystem
{
	public:
		void Init();

		bool Is(RenderState STATE);
		void Set(RenderState STATE, bool value);
		void Revert(RenderState STATE);
		bool Toggle(RenderState STATE);

		void SetGlobalCulling(OpenGL::CULL faces);
		void DisableGlobalCulling();
		void CullFace(OpenGL::CULL faces);
		void RevertPreviousCulling();

		void SetLineWidth(float width = 1.0f);

	private:
		void UpdateGlobalState();
		void SavePreviousState(RenderState STATE);

	public:
		int debugParam;

	private:
		OpenGL::CULL culling;
		OpenGL::CULL prevCulling;

		float lineWidth;

		bool globalCulling;
		bool *states;
		bool *prevStates;
};

