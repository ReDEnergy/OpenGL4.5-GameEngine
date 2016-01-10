#pragma once

namespace _EventType {
	enum E {
		NO_EVENT,

		// Gameplay
		OPEN_GAME_MENU,
		CLOSE_MENU,
		RESUME_GAMEPLAY,
		EXIT_GAME,

		// Animation
		STOP_ANIMATION,

		// Camera
		GAME_CAMERA,
		DEBUG_CAMERA,
		SWITCH_CAMERA,

		// Loop update
		FRAME_START,
		FRAME_BEFORE_RENDERING,
		FRAME_AFTER_RENDERING,
		FRAME_UPDATE,
		FRAME_END,

		// Timings
		TIMER_EXPIRED,

		// SOUND EVENT
		PLAY_SOUND_FX,
		STOP_SOUND_FX,

		// Editor
		EDITOR_OBJECT_ADDED,
		EDITOR_OBJECT_REMOVED,
		EDITOR_OBJECT_SELECTED,
		EDITOR_OBJECT_DELETED,
		EDITOR_NO_SELECTION,

		// SIZE
		SIZE
	};
}

typedef _EventType::E EventType;