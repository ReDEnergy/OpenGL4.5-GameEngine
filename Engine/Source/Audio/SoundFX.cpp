#include "SoundFX.h"

#include <include/gl.h>
#include <Manager/Manager.h>
#include <Manager/EventSystem.h>

SoundFX::SoundFX(sf::SoundBuffer &buffer)
{
	offset = 0;
	sound.setBuffer(buffer);
	duration = sound.getBuffer()->getDuration().asSeconds();
	sound3D = sound.getBuffer()->getChannelCount() > 1 ? false : true;
}

SoundFX::SoundFX(sf::SoundBuffer &buffer, float offset, float duration)
	: offset(offset), duration(duration)
{
	sound.setBuffer(buffer);
	float length = sound.getBuffer()->getDuration().asSeconds();
	if (duration > length)
		duration = length;
	sound3D = sound.getBuffer()->getChannelCount() > 1 ? false : true;
}

SoundFX::~SoundFX()
{

}

void SoundFX::Play()
{
	sound.setPlayingOffset(sf::seconds(offset));
	sound.play();
	sound.setVolume(100);
	Manager::Event->TriggerEvent(this, EventType::STOP_SOUND_FX, NULL, duration);
}

void SoundFX::Update()
{
}

void SoundFX::Pause()
{
	sound.pause();
}

void SoundFX::OnEvent(EventType Event, Object *data)
{
	if (Event == EventType::STOP_SOUND_FX)
		Pause();
}
