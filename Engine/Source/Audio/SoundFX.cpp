#ifdef USE_AUDIO_MANAGER
#include "SoundFX.h"

#include <Manager/Manager.h>
#include <Manager/EventSystem.h>
#include <Event/TimerEvent.h>

SoundFX::SoundFX(sf::SoundBuffer *buffer)
	: offset(0), buffer(buffer)
{
	sound.setBuffer(*buffer);
	duration = sound.getBuffer()->getDuration().asSeconds();
	sound3D = sound.getBuffer()->getChannelCount() > 1 ? false : true;

	fxEvent = Manager::Event->GetStandardTimers()->Create(EventType::STOP_SOUND_FX, duration);
	SubscribeToEvent(EventType::STOP_SOUND_FX);
}

SoundFX::SoundFX(sf::SoundBuffer *buffer, float offset, float duration)
	: offset(offset), duration(duration), buffer(buffer)
{
	sound.setBuffer(*buffer);
	float length = sound.getBuffer()->getDuration().asSeconds();
	if (duration > length)
		duration = length;
	sound3D = sound.getBuffer()->getChannelCount() > 1 ? false : true;

	fxEvent = Manager::Event->GetStandardTimers()->Create(EventType::STOP_SOUND_FX, duration);
	SubscribeToEvent(EventType::STOP_SOUND_FX);
}

SoundFX::~SoundFX()
{
}

void SoundFX::SetVolume(unsigned int volume)
{
	sound.setVolume((float)volume);
}

float SoundFX::GetVolume() const
{
	return volume;
}

short int* SoundFX::GetBuffer() const
{
	return (short int*)buffer->getSamples();
}

float SoundFX::GetDuration() const
{
	return duration;
}

unsigned int SoundFX::GetChannelCount() const
{
	return buffer->getChannelCount();
}

unsigned int SoundFX::GetSampleRate() const
{
	return buffer->getSampleRate();
}

void SoundFX::Play()
{
	sound.setPlayingOffset(sf::seconds(offset));
	sound.play();
	Manager::Event->GetStandardTimers()->Add(*fxEvent);
}

void SoundFX::Update()
{
}

void SoundFX::Pause()
{
	sound.pause();
}

void SoundFX::OnEvent(EventType Event, void *data)
{
	if (Event == EventType::STOP_SOUND_FX) {
		Manager::Event->GetStandardTimers()->Remove(*fxEvent);
		Pause();
	}
}
#endif