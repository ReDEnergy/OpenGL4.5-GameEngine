#pragma once

#include <include/dll_export.h>

using namespace std;

namespace ERenderState {
	enum RS {
			DEBUG,
			WIREFRAME,
			SHADOW,
			FORWARD,
			SS_AO,
			DEFERRED,
			POST_PROCESS,
			HIDE_POINTER,
			CLIP_POINTER,
			VSYNC
	};
}
typedef ERenderState::RS RenderState;

class DLLExport RenderingSystem
{
	public:
		void Init();
		bool Is(RenderState STATE);
		void Set(RenderState STATE, bool value);
		void Revert(RenderState STATE);
		bool Toggle(RenderState STATE);

	private:
		void UpdateGlobalState();
		void SavePreviousState(RenderState STATE);

	public:
		int debugParam;

	private:
		bool *states;
		bool *prevStates;
};