#pragma once

namespace _EventType {
	enum E {
		// Gameplay
		OPEN_GAME_MENU,
		CLOSE_MENU,
		RESUME_GAMEPLAY,
		PLAY_SOUND_FX,
		WARNING_COLLECT_EVERYTHING,
		LEVEL_FISIHED,
		LEVEL_TRANISTION_GATE,
		EXIT_GAME,

		// Camera
		GAME_CAMERA,
		DEBUG_CAMERA,
		SWITCH_CAMERA,

		// Orbs
		CREATE_ORB,
		COLLECT_ORB,

		// Debug
		DEBUG_BARREL_SPAWN,

		// Overlay
		UPDATE_SCORE,
		CREATE_SCORE,

		// SIZE
		SIZE
	};
}

typedef _EventType::E EventType;