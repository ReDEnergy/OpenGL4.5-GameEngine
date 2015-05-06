#pragma once

#include <list>

#include <include/dll_export.h>
#include <include/glm.h>

#include <SFML/Audio.hpp>
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
		void SetVolume(float value);
		void SetStatus(bool state);
		void ToggleStatus();
		void SetPosition(glm::vec3 position);

	public:
		sf::Music music;
		string name;
};