#pragma once
#include "stdafx.h"

#include <memory>

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
    Game(Game&);
    Game& operator=(Game&);

    void Load();
    void LoadNextLevel();

    sf::RenderWindow* m_Window;
    ScoreManager* m_ScoreManager;
    std::shared_ptr<Level> m_Level;
    Hud* m_Hud;

    int m_LevelMax;
};
