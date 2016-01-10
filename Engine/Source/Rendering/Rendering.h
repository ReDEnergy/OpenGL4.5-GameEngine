#pragma once

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

namespace OpenGL {
	enum class CULL {
		FRONT,
		BACK,
		BOTH,
		NONE
	};
};