#include "stdafx.h"
#include "Hud.h"

#include "ScoreManager.h"

Hud::Hud(ScoreManager const& scoreManager) :
    m_ScoreManager(scoreManager)
{
}

Hud::~Hud()
{
}

void Hud::Load()
{
    if (!m_Font.loadFromFile("sansation.ttf"))
    {
        assert(false);
    }

    m_Life.Load("Player/Player_06");
}

void Hud::Draw(sf::RenderTarget& target)
{
    sf::Text text;
    text.setFont(m_Font);

    char msg[32];

    if (m_ScoreManager.GetReady() || m_ScoreManager.GetGameOver())
    {
        if (m_ScoreManager.GetGameOver())
        {
            text.setString("Game Over");
        }
        else
        {
            text.setString("Get Ready");
        }
        text.setCharacterSize(36);
        float const x = 0.5f * (target.getSize().x - text.getLocalBounds().width);
        text.setPosition(x, 8.0f);
        target.draw(text);
    }

    // Score
    text.setString("Score");
    text.setCharacterSize(24);
    text.setPosition(12.0f, 8.0f);
    target.draw(text);

    sprintf_s(msg, "%d", m_ScoreManager.GetPoints());
    text.setString(msg);
    text.setCharacterSize(48);
    text.setPosition(12.0f, 28.0f);
    target.draw(text);

    // Level
    text.setString("Level");
    text.setCharacterSize(24);
    text.setPosition(12.0f, 98.0f);
    target.draw(text);

    sprintf_s(msg, "%d", m_ScoreManager.GetLevel());
    text.setString(msg);
    text.setCharacterSize(48);
    text.setPosition(12.0f, 118.0f);
    target.draw(text);

    // Lives
    text.setString("Lives");
    text.setCharacterSize(24);
    text.setPosition(12.0f, 188.0f);
    target.draw(text);

    float lifeX = 12.0f;
    for (int i = 0; i < m_ScoreManager.GetLives(); ++i)
    {
        m_Life.SetPosition(lifeX, 218.0f);
        m_Life.Draw(target);
        lifeX += 40.0f;
    }
}
