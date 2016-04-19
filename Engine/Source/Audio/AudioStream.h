#pragma once

#if defined(USE_AUDIO_MANAGER)

#include <list>
#include <string>

#include <include/dll_export.h>
#include <include/glm.h>

#if defined(USE_AUDIO_MANAGER) && defined(ENGINE_DLL_EXPORTS)
#include <SFML/Audio.hpp>
#endif
#include <Component/AudioSource.h>

class DLLExport AudioStream : public AudioSource
{
	public:
		AudioStream(const std::string &file);
		~AudioStream();

		void Play();
		void Pause();
		void Stop();

		bool IsPlaying() const;
		float GetVolume() const;
		float GetDuration() const;

		void SetLoop(bool state);
		void SetVolume(float value);
		void SetStatus(bool state);
		void ToggleStatus();
		void SetPosition(glm::vec3 position);
		void PreloadStream();

	private:
		void LoadStream();

	public:
		bool loaded;
		std::string file;
		#if defined(USE_AUDIO_MANAGER) && defined(ENGINE_DLL_EXPORTS)
		sf::Music music;
		#endif
		std::string name;
};

#endif