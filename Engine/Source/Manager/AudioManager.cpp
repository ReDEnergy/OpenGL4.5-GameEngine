//#include <pch.h>
#include "AudioManager.h"

#include <string>

#include <include/gl.h>
#include <include/utils.h>

#include <Audio/AudioStream.h>
#include <Audio/SoundFX.h>

#include <Manager/DebugInfo.h>
#include <Manager/Manager.h>
#include <Component/Transform/Transform.h>


AudioManager::AudioManager() {
}

AudioManager::~AudioManager() {

}

void AudioManager::Init() {
	Manager::Debug->InitManager("Audio");
}

void AudioManager::LoadAudio(const string &fileLocation, const string &UID, AUDIO_TYPE TYPE)
{
	switch (TYPE) {

		case AUDIO_TYPE::MUSIC: {
			AudioStream *music = new AudioStream(fileLocation);
			audioStreams[UID] = music;
			return;
		}

		case AUDIO_TYPE::SOUND_FX_FILE: {
			sf::SoundBuffer *buffer = new sf::SoundBuffer();
			if (!buffer->loadFromFile(fileLocation))
				printf("Error opening sound-fx file: %s \n", fileLocation.c_str());

			soundBuffers[UID] = buffer;
			return;
		}
	}
}

void AudioManager::InitSoundFX(const string &buffer, const string &name, float startTime, float duration)
{
	cout << buffer.c_str() << " " << name.c_str() << " " << startTime << " " << duration << endl;
	sf::SoundBuffer *sb = soundBuffers[buffer];
	soundEffects[name] = new SoundFX(*sb, startTime, duration);
}

AudioSource* AudioManager::GetAudioSource(const string &name)
{
	return audioStreams[name];
}

void AudioManager::Update(Camera *player)
{
	glm::vec3 pos = player->transform->GetWorldPosition();
	glm::vec3 forward = player->transform->GetLocalOZVector();
	sf::Listener::setDirection(forward.x, forward.y, forward.z);
	sf::Listener::setPosition(pos.x, pos.y, pos.z);
}

void AudioManager::PlayStream(const char *streamUID) {
	AudioStream *music = audioStreams[streamUID];
	if (music)
		music->Play();
}

void AudioManager::PlaySoundFX(const char *soundFX_UID) {
	SoundFX *sound = soundEffects[soundFX_UID];
	if (sound)
		sound->Play();
}