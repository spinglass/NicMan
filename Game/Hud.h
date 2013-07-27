#pragma once

#include "Core/Sprite.h"

class ScoreManager;

class Hud
{
public:
    Hud(ScoreManager const& scoreManager);
    virtual ~Hud();

    void Load();

    void Draw(sf::RenderTarget& target);

private:
    Hud(Hud&);
    Hud& operator=(Hud&);

    ScoreManager const& m_ScoreManager;
    sf::Font m_Font;
    Sprite m_Life;
};
