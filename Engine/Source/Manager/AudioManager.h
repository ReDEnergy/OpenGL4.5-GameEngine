#pragma once

#include <list>
#include <unordered_map>

#include <include/dll_export.h>
#include <include/utils.h>

#if defined(USE_AUDIO_MANAGER) && defined(ENGINE_DLL_EXPORTS)
#include <SFML/Audio.hpp>
#endif

#include <Core/Camera/Camera.h>

enum class AUDIO_TYPE {
	MUSIC,
	SOUND_FX_FILE,
	EFFECT,
};

class DLLExport AudioManager
{
	protected:
		AudioManager();
		~AudioManager();

	public:
		void Init();
		void Update(Camera *player);
	
		void PlaySoundFX(const char *soundFX_UID);

		void LoadAudio(const std::string &fileLocation, const std::string &UID, AUDIO_TYPE TYPE);
		void InitSoundFX(const std::string &buffer, const std::string &name, float offset, float duration);

		AudioSource* GetSoundEffect(const std::string & name) const;
		AudioSource* GetAudioSource(const std::string &name) const;

	private:

		#if defined(USE_AUDIO_MANAGER) && defined(ENGINE_DLL_EXPORTS)

		std::unordered_map <std::string, AudioSource*> soundEffects;
		std::unordered_map <std::string, AudioSource*> audioStreams;
		std::unordered_map <std::string, sf::SoundBuffer*> soundBuffers;

		#endif
};
