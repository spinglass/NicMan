#include "stdafx.h"
#include "Sound.h"

Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::Load(char const* filename)
{
    if (m_Buffer.loadFromFile(filename))
    {
        m_Sound = sf::Sound(m_Buffer);
    }
}

void Sound::SetVolume(float vol)
{
    m_Sound.setVolume(vol);
}

void Sound::Play()
{
    m_Sound.play();
}
