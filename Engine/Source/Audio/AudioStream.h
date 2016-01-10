#pragma once

#include <list>

#include <include/dll_export.h>
#include <include/glm.h>

#ifdef ENGINE_DLL_EXPORTS
#include <SFML/Audio.hpp>
#endif
#include <Component/AudioSource.h>

using namespace std;

class AudioStream : public AudioSource
{
	public:
		AudioStream();
		AudioStream(const string &file);
		~AudioStream();

		void Play();
		void Stop();
		void SetLoop(bool state);
		void SetVolume(unsigned int value);
		void SetStatus(bool state);
		void ToggleStatus();
		void SetPosition(glm::vec3 position);

	public:
		#ifdef ENGINE_DLL_EXPORTS
		sf::Music music;
		#endif
		string name;
};