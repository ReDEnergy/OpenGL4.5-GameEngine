#pragma once

#include <list>
#include <unordered_map>

#include <include/dll_export.h>
#include <SFML/Audio.hpp>

using namespace std;

class SoundFX {
	public:
		SoundFX();
		~SoundFX();

		void Update();
		void Play();
		void Pause();

	public:		
		double startTime;
		float offset;
		float duration;
		float volume;
		sf::Sound *sound;
};

class DLLExport AudioManager
{
	protected:
		AudioManager();
		~AudioManager();

	public:
		void Init();
		void Update();
		void PlaySoundFX(const char *soundFX_UID);

	public:
		void Play();

	private:
		bool play;
		sf::Music music;
		sf::SoundBuffer buffer;
		sf::Sound *sound;
		list <SoundFX*> toUpdate;
		list <SoundFX*> toRemove;
		unordered_map <string, SoundFX*> effects;
};