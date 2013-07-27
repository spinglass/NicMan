#pragma once

#include "Sprite.h"
#include "Entities/Ghost.h"
#include "Entities/Player.h"
#include "Maze/Maze.h"

class Level
{
public:
    Level();
    virtual ~Level();

    void Load(char* filename);

    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    void UpdateBehaviour(float dt);
    void UpdatePowerPlay(float dt);

    Maze m_Maze;

    Player m_Player;
    std::vector<std::shared_ptr<Ghost>> m_Ghosts;

    std::vector<float> m_BehaviourChanges;
    int m_BehaviourCounter;
    float m_BehaviourTimer;
    Ghost::Behaviour m_MainBehaviour;

    float m_PowerTimer;
};
