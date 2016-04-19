#pragma once
#ifdef USE_AUDIO_MANAGER

#include <list>
#ifdef ENGINE_DLL_EXPORTS
#include <SFML/Audio.hpp>
#endif

#include <include/dll_export.h>

#include <Component/AudioSource.h>
#include <Event/EventListener.h>

template <class T>
class TimerEvent;

class DLLExport SoundFX:
	public AudioSource, 
	public EventListener
{
	public:
		#ifdef ENGINE_DLL_EXPORTS
		SoundFX(sf::SoundBuffer *buffer);
		SoundFX(sf::SoundBuffer *buffer, float offset, float duration);
		#endif
		~SoundFX();

		void Update();
		void Play();
		void Pause();

		float GetVolume() const;
		float GetDuration() const;
		unsigned int GetChannelCount() const;
		unsigned int GetSampleRate() const;
		short int* GetBuffer() const;

		void SetVolume(unsigned int value);

	private:
		void OnEvent(EventType Event, void *data);

	public:
		float offset;
		float duration;
		float volume;

		#ifdef ENGINE_DLL_EXPORTS
		sf::Sound sound;
		sf::SoundBuffer *buffer;
		#endif

	private:
		double startTime;
		TimerEvent<EventType> *fxEvent;
};

#endif