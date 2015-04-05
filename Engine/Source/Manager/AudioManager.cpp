//#include <pch.h>
#include "AudioManager.h"

#include <string>

#include <include/gl.h>
#include <include/utils.h>
#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>

static const string A_PATH("Resources\\Audio\\");

AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {

}

void AudioManager::Init() {
	Manager::Debug->InitManager("Audio");

	/*
	if (!music.openFromFile(A_PATH + "relax.ogg"))
		printf("error loading audio.ogg");

	music.setVolume(75);

	if (!buffer.loadFromFile(A_PATH + "Effects\\" + "music box.ogg"))
			printf("error loading 'music box.ogg'");
	
	sound = new sf::Sound();
	sound->setBuffer(buffer);

	SoundFX *effect = new SoundFX();
	effect->sound = sound;
	effect->offset = 4.3f;
	effect->duration = 0.4f;
	effect->volume = 25;

	effects["bell"] = effect;
	*/
}


void AudioManager::Update() {
	for (auto item : toUpdate) {
		item->Update();
	}
	for (auto item : toRemove) {
		toUpdate.remove(item);
	}
	toRemove.clear();
};

void AudioManager::Play() {
	printf("Audio: %d", play ? 1 : 0);
	play ? music.play() : music.pause();
	play = !play;
}

void AudioManager::PlaySoundFX(const char *soundFX_UID) {
	SoundFX *sound = effects[soundFX_UID];
	if (sound)
		sound->Play();
}

SoundFX::SoundFX() {
}

void SoundFX::Play() {
	sound->setPlayingOffset(sf::seconds(offset));
	sound->play();
	sound->setVolume(30);
	startTime = glfwGetTime();
	// toUpdate.push_back(this);
}

void SoundFX::Update() {
	float elapsed = float(glfwGetTime() - startTime);
	float fade = (duration - elapsed) / duration;
	sound->setVolume(fade * volume + 10);
	if (elapsed > duration)
		Pause();
}

void SoundFX::Pause() {
	sound->pause();
	// toRemove.push_back(this);
}

// Inspect whether or not the SoudFX should be a component
// How to treat sound location