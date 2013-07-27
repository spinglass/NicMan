#pragma once

#include <array>

#include "Core/Sprite.h"
#include "Entities/Ghost.h"
#include "Entities/Player.h"
#include "Maze/Maze.h"

class ScoreManager;
struct LevelSettings;

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

    static float const s_MaxSpeed;
    static float const s_StartWait;
    static float const s_EatWait;
    static float const s_DeathWait;
    static float const s_CompletionWait;

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

    std::shared_ptr<LevelSettings> m_Settings;

    int m_ScorePill;
    int m_ScorePowerPill;
    std::array<int, 4> m_ScoreGhosts;
    int m_GhostEatCount;
    std::array<Sprite, 4> m_ScoreSprites;
    Ghost const* m_EatenGhost;
};
