#include "AudioManager.h"

#include <string>
#include <iostream>
using namespace std;

#include <include/utils.h>

#ifdef USE_AUDIO_MANAGER
#include <Audio/AudioStream.h>
#include <Audio/SoundFX.h>
#endif

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
#ifdef USE_AUDIO_MANAGER

	switch (TYPE) {

		case AUDIO_TYPE::MUSIC: {
			AudioStream *music = new AudioStream(fileLocation);
			audioStreams[UID] = music;
            break;
		}

		case AUDIO_TYPE::SOUND_FX_FILE: {
			sf::SoundBuffer *buffer = new sf::SoundBuffer();
			if (!buffer->loadFromFile(fileLocation))
				printf("Error opening sound-fx file: %s \n", fileLocation.c_str());

			soundBuffers[UID] = buffer;
            break;
		}

		default: {
            break;
		}
	}
#endif
}

void AudioManager::InitSoundFX(const string &buffer, const string &name, float startTime, float duration)
{
#ifdef USE_AUDIO_MANAGER

	#ifdef _DEBUG
	cout << buffer.c_str() << " " << name.c_str() << " " << startTime << " " << duration << endl;
	#endif
	auto it = soundBuffers.find(buffer);
	if (it != soundBuffers.end())
	{
		soundEffects[name] = new SoundFX(it->second, startTime, duration);
	}
	else {
		cout << "[AudioManager] Sound BUffer not found" << endl;
	}
#endif
}

AudioSource* AudioManager::GetAudioSource(const string &name) const
{
#ifdef USE_AUDIO_MANAGER
	auto it = audioStreams.find(name);
	if (it != audioStreams.end())
		return it->second;
#endif
	return nullptr;
}

AudioSource* AudioManager::GetSoundEffect(const string &name) const
{
#ifdef USE_AUDIO_MANAGER
	auto it = soundEffects.find(name);
	if (it != soundEffects.end())
		return it->second;
#endif
	return nullptr;
}

void AudioManager::Update(Camera *player)
{
#ifdef USE_AUDIO_MANAGER
	glm::vec3 pos = player->transform->GetWorldPosition();
	glm::vec3 forward = player->transform->GetLocalOZVector();
	sf::Listener::setDirection(forward.x, forward.y, forward.z);
	sf::Listener::setPosition(pos.x, pos.y, pos.z);
#endif
}

void AudioManager::PlaySoundFX(const char *soundFX_UID)
{
#ifdef USE_AUDIO_MANAGER
	auto sound = soundEffects[soundFX_UID];
	if (sound)
		sound->Play();
#endif
}