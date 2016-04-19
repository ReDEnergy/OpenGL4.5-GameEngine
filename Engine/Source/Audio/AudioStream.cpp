#include "AudioStream.h"
#ifdef USE_AUDIO_MANAGER

using namespace std;

AudioStream::AudioStream(const string& file)
{
	sound3D = false;
	loaded = false;
	this->file = file;
}

AudioStream::~AudioStream()
{
}

void AudioStream::Play()
{
	if (!loaded) LoadStream();
	music.play();
}

void AudioStream::Pause()
{
	if (!loaded) return;
	music.pause();
}

void AudioStream::Stop()
{
	if (!loaded) return;
	music.stop();
}

bool AudioStream::IsPlaying() const
{
	if (!loaded) return false;
	return (music.getStatus() == sf::SoundSource::Status::Playing);
}

float AudioStream::GetVolume() const
{
	if (!loaded) return 0;
	return music.getVolume();
}

float AudioStream::GetDuration() const
{
	if (!loaded) return 0;
	return music.getDuration().asSeconds();
}

void AudioStream::SetLoop(bool state)
{
	if (!loaded) return;
	music.setLoop(state);
}

void AudioStream::SetVolume(float value)
{
	if (!loaded) return;
	music.setVolume(value);
}

void AudioStream::SetStatus(bool status)
{
	if (!loaded) return;
	status ? music.play() : music.stop();
}

void AudioStream::ToggleStatus()
{
	if (!loaded) return;
	(music.getStatus() == sf::SoundSource::Paused) ? music.play() : music.pause();
}

void AudioStream::SetPosition(glm::vec3 position)
{
	if (!loaded) return;
	music.setPosition(position.x, position.y, position.z);
}

void AudioStream::PreloadStream()
{
	LoadStream();
}

void AudioStream::LoadStream()
{
	if (loaded)
		return;

	loaded = true;

	if (!music.openFromFile(file))
		printf("Error opening music file: %s \n", file.c_str());

	sound3D = music.getChannelCount() > 1 ? false : true;
}

#endif