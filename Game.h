#pragma once
#include "stdafx.h"

class Level;

class Game
{
public:
    Game();
    virtual ~Game();

    void Run();

private:
    sf::RenderWindow* m_Window;
    Level* m_Level;
};
