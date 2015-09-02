#pragma once

namespace _EventType {
	enum E {
		// Gameplay
		OPEN_GAME_MENU,
		CLOSE_MENU,
		RESUME_GAMEPLAY,
		WARNING_COLLECT_EVERYTHING,
		LEVEL_FISIHED,
		LEVEL_TRANISTION_GATE,
		EXIT_GAME,

		// Animation
		STOP_ANIMATION,

		// Camera
		GAME_CAMERA,
		DEBUG_CAMERA,
		SWITCH_CAMERA,

		// Orbs
		CREATE_ORB,
		COLLECT_ORB,

		// Overlay
		UPDATE_SCORE,
		CREATE_SCORE,

		// SOUND EVENT
		PLAY_SOUND_FX,
		STOP_SOUND_FX,

		// Editor
		EDITOR_OBJECT_SELECTED,

		// SIZE
		SIZE
	};
}

typedef _EventType::E EventType;