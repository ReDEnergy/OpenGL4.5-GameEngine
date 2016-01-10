#pragma once

#include <list>
#include <unordered_map>

#include <include/dll_export.h>
#include <include/utils.h>

#ifdef ENGINE_DLL_EXPORTS
#include <SFML/Audio.hpp>
#endif
#include <Core/Camera/Camera.h>

using namespace std;

class AudioStream;
class SoundFX;

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
	
		void PlayStream(const char *streamUID);
		void PlaySoundFX(const char *soundFX_UID);

		void LoadAudio(const string &fileLocation, const string &UID, AUDIO_TYPE TYPE);
		void InitSoundFX(const string &buffer, const string &name, float offset, float duration);

		SoundFX* GetSoundEffect(const string & name) const;
		AudioSource* GetAudioSource(const string &name) const;

	private:
		unordered_map <string, SoundFX*> soundEffects;
		unordered_map <string, AudioStream*> audioStreams;
		#ifdef ENGINE_DLL_EXPORTS
		unordered_map <string, sf::SoundBuffer*> soundBuffers;
		#endif
};