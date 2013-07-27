#pragma once

#include <array>

#include "Core/Sprite.h"
#include "Entities/Ghost.h"
#include "Entities/Player.h"
#include "Maze/Maze.h"

class ScoreManager;

class Level
{
public:
    Level(ScoreManager& scoreManager);
    virtual ~Level();

    void Load(char* filename);

    bool IsComplete() const { return m_Complete; }

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
        Complete,
    };

    Level(Level&);
    Level& operator=(Level&);

    void UpdateStart(float dt);
    void UpdateNormal(float dt);
    void UpdateFright(float dt);
    void UpdateEat(float dt);
    void UpdateDeath(float dt);
    void UpdateComplete(float dt);

    void UpdateEntities(float dt);

    void Restart();

    ScoreManager& m_ScoreManager;
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
    bool m_Complete;

    float m_MaxSpeed;
    float m_PlayerNormSpeedFactor;
    float m_PlayerFrightSpeedFactor;
    float m_GhostNormSpeedFactor;
    float m_GhostFrightSpeedFactor;
    float m_GhostTunnelSpeedFactor;

    int m_ScorePill;
    int m_ScorePowerPill;
    std::array<int, 4> m_ScoreGhosts;
    int m_GhostEatCount;
};
