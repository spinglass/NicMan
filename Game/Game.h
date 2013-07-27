#pragma once
#include "stdafx.h"

class Hud;
class Level;
class ScoreManager;

class Game
{
public:
    Game();
    virtual ~Game();

    void Run();

private:
    sf::RenderWindow* m_Window;
    ScoreManager* m_ScoreManager;
    Level* m_Level;
    Hud* m_Hud;
};
