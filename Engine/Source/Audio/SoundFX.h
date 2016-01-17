#pragma once

#include <list>
#ifdef ENGINE_DLL_EXPORTS
#include <SFML/Audio.hpp>
#endif

#include <include/dll_export.h>

#include <Component/AudioSource.h>
#include <Event/EventListener.h>

using namespace std;

template <class T>
class TimerEvent;

class SoundFX : 
	public AudioSource, 
	public EventListener
{
	public:
		#ifdef ENGINE_DLL_EXPORTS
		SoundFX(sf::SoundBuffer &buffer);
		SoundFX(sf::SoundBuffer &buffer, float offset, float duration);
		#endif
		~SoundFX();

		void Update();
		void Play();
		void Pause();
		void OnEvent(EventType Event, void *data);
		void SetVolume(unsigned int value);
		float GetVolume() const;

	public:
		float offset;
		float duration;
		float volume;

		#ifdef ENGINE_DLL_EXPORTS
		sf::Sound sound;
		sf::SoundBuffer buffer;
		#endif

	private:
		double startTime;
		TimerEvent<EventType> *fxEvent;
};
