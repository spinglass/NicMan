#pragma once

#include "Core/Sprite.h"
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
    enum class State
    {
        Start,
        Normal,
        Fright,
        Eat,
        Death,
    };

    void UpdateStart(float dt);
    void UpdateNormal(float dt);
    void UpdateFright(float dt);
    void UpdateEat(float dt);
    void UpdateDeath(float dt);

    void UpdateEntities(float dt);

    void Restart();

    Maze m_Maze;

    Player m_Player;
    std::vector<std::shared_ptr<Ghost>> m_Ghosts;

    std::vector<float> m_BehaviourChanges;
    int m_BehaviourCounter;
    float m_BehaviourTimer;
    State m_State;
    float m_WaitTimer;
    Ghost::Behaviour m_NormalBehaviour;
    float m_FrightTimer;

    float m_MaxSpeed;
    float m_PlayerNormSpeedFactor;
    float m_PlayerFrightSpeedFactor;
    float m_GhostNormSpeedFactor;
    float m_GhostFrightSpeedFactor;
};
