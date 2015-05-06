#pragma once

#include <list>
#include <SFML/Audio.hpp>

#include <include/dll_export.h>

#include <Component/AudioSource.h>
#include <Event/EventListener.h>

using namespace std;

class SoundFX : 
	public AudioSource, 
	public EventListener
{
	public:
		SoundFX(sf::SoundBuffer &buffer);
		SoundFX(sf::SoundBuffer &buffer, float offset, float duration);
		~SoundFX();

		void Update();
		void Play();
		void Pause();
		void OnEvent(EventType Event, Object *data);

	public:
		float offset;
		float duration;
		float volume = 100;

		sf::Sound sound;
		sf::SoundBuffer buffer;

	private:
		double startTime;
};
