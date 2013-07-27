#pragma once

class Sound
{
public:
    Sound();
    virtual ~Sound();

    void Load(char const* filename);

    void SetVolume(float vol);

    void Play();

private:
    sf::SoundBuffer m_Buffer;
    sf::Sound m_Sound;
};
