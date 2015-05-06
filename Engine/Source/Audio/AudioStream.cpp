#include "AudioStream.h"

AudioStream::AudioStream()
{

}

AudioStream::AudioStream(const string &file)
{
	if (!music.openFromFile(file))
		printf("Error opening music file: %s \n", file.c_str());

	sound3D = music.getChannelCount() > 1 ? false : true;
}

AudioStream::~AudioStream()
{

}

void AudioStream::Play() {
	music.play();
}

void AudioStream::Stop()
{
	music.stop();
}

void AudioStream::SetLoop(bool state)
{
	music.setLoop(state);
}

void AudioStream::SetVolume(float value)
{
	music.setVolume(value);
}

void AudioStream::SetStatus(bool status)
{
	status ? music.play() : music.stop();
}

void AudioStream::ToggleStatus()
{
	(music.getStatus() == sf::SoundSource::Paused) ? music.play() : music.pause();
}

void AudioStream::SetPosition(glm::vec3 position)
{
	music.setPosition(position.x, position.y, position.z);
}
