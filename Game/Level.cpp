#include "stdafx.h"
#include "Level.h"

#include "Maze/Cell.h"
#include "GhostTargets/BlinkyTarget.h"
#include "GhostTargets/PinkyTarget.h"
#include "GhostTargets/InkyTarget.h"
#include "GhostTargets/ClydeTarget.h"
#include "GhostTargets/FixedTarget.h"
#include "ScoreManager.h"

Level::Level(ScoreManager& scoreManager) :
    m_ScoreManager(scoreManager),
    m_BehaviourCounter(0),
    m_BehaviourTimer(0.0f),
    m_State(State::Start),
    m_WaitTimer(2.0f),
    m_FrightTimer(0.0f),
    m_Complete(false),
    m_NormalBehaviour(Ghost::Behaviour::Scatter),
    m_MaxSpeed(8.0f),
    m_PlayerNormSpeedFactor(0.8f),
    m_PlayerFrightSpeedFactor(0.9f),
    m_GhostNormSpeedFactor(0.75f),
    m_GhostFrightSpeedFactor(0.5f),
    m_GhostTunnelSpeedFactor(0.4f),
    m_ScorePill(10),
    m_ScorePowerPill(50),
    m_GhostEatCount(0)
{
    m_ScoreGhosts[0] = 200;
    m_ScoreGhosts[1] = 400;
    m_ScoreGhosts[2] = 800;
    m_ScoreGhosts[3] = 1600;
}

Level::~Level()
{
}

void Level::Load(char* filename)
{
    m_Maze.Load(filename);

    m_Player.Load();

    m_Ghosts.resize(4);
    for (int i = 0; i < m_Ghosts.size(); ++i)
    {
        m_Ghosts[i] = std::make_shared<Ghost>();
        m_Ghosts[i]->Load(i);
    }

    std::shared_ptr<IGhostTarget> eatenTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(13, 19));
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<BlinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(25, 32));

        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<PinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(2, 32));

        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<InkyTarget>(m_Player.GetMovement(), m_Ghosts[0]->GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(27, -1));

        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<ClydeTarget>(m_Player.GetMovement(), m_Ghosts[3]->GetMovement(), m_Maze.GetGridRef(0, -1));
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef( 0, -1));

        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
    }

    // Setup chase/scatter cycle
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(5.0f);

    Restart();
}

void Level::Update(float dt)
{
    switch(m_State)
    {
    case State::Start:
        UpdateStart(dt);
        break;
    case State::Normal:
        UpdateNormal(dt);
        break;
    case State::Fright:
        UpdateFright(dt);
        break;
    case State::Eat:
        UpdateEat(dt);
        break;
    case State::Death:
        UpdateDeath(dt);
        break;
    case State::Complete:
        UpdateComplete(dt);
        break;
    }
}

void Level::UpdateStart(float dt)
{
    m_WaitTimer -= dt;
    bool go = (m_WaitTimer < 0.0f);
    if (go)
    {
        m_State = State::Normal;
    }
    m_ScoreManager.SetReady(!go);
}

void Level::UpdateNormal(float dt)
{
    // Check for swapping between scatter and chase
    if (m_BehaviourCounter < m_BehaviourChanges.size())
    {
        m_BehaviourTimer += dt;
        if (m_BehaviourTimer > m_BehaviourChanges[m_BehaviourCounter])
        {
            // Swap behaviour
            m_NormalBehaviour = (m_NormalBehaviour == Ghost::Behaviour::Chase) ? Ghost::Behaviour::Scatter : Ghost::Behaviour::Chase;

            // Reset for next change
            m_BehaviourTimer = 0.0f;
            ++m_BehaviourCounter;
        }
    }

    // Apply to all ghosts
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        ghost->SetBehaviour(m_NormalBehaviour);
    }

    UpdateEntities(dt);
}

void Level::UpdateFright(float dt)
{
    // Update flash
    if (m_FrightTimer < 1.0f)
    {
        // Detemine flash state
        bool const frightFlash = (int)floorf(4.0f * m_FrightTimer) % 2 == 1;

        // Apply to all ghosts
        for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
        {
            ghost->SetFrightFlash(frightFlash);
        }
    }

    UpdateEntities(dt);

    // Check for eating a ghost
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        if (ghost->GetBehaviour() == Ghost::Behaviour::Fright &&
            m_Player.GetMovement().GetPosition() == ghost->GetMovement().GetPosition())
        {
            // Nom!
            ghost->SetBehaviour(Ghost::Behaviour::Eaten);

            // Score and increase score for next eat
            m_ScoreManager.Add(m_ScoreGhosts[m_GhostEatCount]);
            m_GhostEatCount = std::min(m_GhostEatCount + 1, (int)m_ScoreGhosts.size() - 1);

            // Switch to eat state for a short while
            m_State = State::Eat;
            m_WaitTimer = 1.0f;
        }
    }

    // Check for finishing fright
    m_FrightTimer -= dt;
    if (m_FrightTimer < 0.0f)
    {
        m_State = State::Normal;
    }
}

void Level::UpdateEat(float dt)
{
    m_WaitTimer -= dt;
    if (m_WaitTimer < 0.0f)
    {
        // Return to fright
        m_State = State::Fright;
    }
}

void Level::UpdateDeath(float dt)
{
    m_WaitTimer -= dt;
    if (m_WaitTimer < 0.0f)
    {
        m_ScoreManager.LoseLife();

        if (!m_ScoreManager.GetGameOver())
        {
            Restart();
        }
    }
}

void Level::UpdateComplete(float dt)
{
    m_WaitTimer -= dt;
    if (m_WaitTimer < 0.0f)
    {
        m_Complete = true;
    }
}

void Level::UpdateEntities(float dt)
{
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        float ghostSpeedFactor = m_GhostNormSpeedFactor;
        if (ghost->GetMovement().GetPosition().GetCell()->IsTunnel())
        {
            ghostSpeedFactor = m_GhostTunnelSpeedFactor;
        }
        else if (ghost->GetBehaviour() == Ghost::Behaviour::Fright)
        {
            ghostSpeedFactor = m_GhostFrightSpeedFactor;
        }
        ghost->SetSpeed(ghostSpeedFactor * m_MaxSpeed);
        ghost->Update(dt);
    }

    float const playerSpeedFactor = (m_State == State::Fright) ? m_PlayerFrightSpeedFactor : m_PlayerNormSpeedFactor;
    m_Player.SetSpeed(playerSpeedFactor * m_MaxSpeed);
    m_Player.Update(dt);

    // Check for player eating a power pill
    if (m_Player.AtePowerPill())
    {
        m_ScoreManager.Add(50);

        if (m_State != State::Fright)
        {
            m_GhostEatCount = 0;
        }

        m_State = State::Fright;
        m_FrightTimer = 6.0f;

        // Apply to all ghosts, unless already eaten
        for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
        {
            if (ghost->GetBehaviour() != Ghost::Behaviour::Eaten)
            {
                ghost->SetBehaviour(Ghost::Behaviour::Fright);
                ghost->SetFrightFlash(false);
            }
        }
    }
    else if (m_Player.AtePill())
    {
        m_ScoreManager.Add(10);
    }

    if (m_Player.AtePowerPill() || m_Player.AtePill())
    {
        if (!m_Maze.GetPillsRemaining())
        {
            // Level finished!
            m_State = State::Complete;
            m_WaitTimer = 2.0f;
        }
    }

    // Check for ghosts eating player
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        if ((ghost->GetBehaviour() == Ghost::Behaviour::Chase || ghost->GetBehaviour() == Ghost::Behaviour::Scatter) &&
            m_Player.GetMovement().GetPosition() == ghost->GetMovement().GetPosition())
        {
            m_State = State::Death;
            m_WaitTimer = 2.0f;
        }
    }
}

void Level::Restart()
{
    m_State = State::Start;
    m_BehaviourCounter = 0;
    m_BehaviourTimer = 0.0f;
    m_WaitTimer = 2.0f;
    m_NormalBehaviour = Ghost::Behaviour::Scatter;
    m_FrightTimer = 0.0f;

    m_Player.Restart(m_Maze.GetGridRef(14, 7), 0.0f, 0.5f);
    m_Ghosts[0]->Restart(m_Maze.GetGridRef(13, 19), 1.0f, 0.5f);
    m_Ghosts[1]->Restart(m_Maze.GetGridRef(4, 22), 0.1f, 0.5f);
    m_Ghosts[2]->Restart(m_Maze.GetGridRef(24, 22), 0.2f, 0.5f);
    m_Ghosts[3]->Restart(m_Maze.GetGridRef(14, 25), 0.3f, 0.5f);
}

void Level::Draw(sf::RenderTarget& target)
{
    float const k_LeftBorder = 260.0f;
    float const k_TopBorder = 50.0f;

    // Border
    sf::RectangleShape border(sf::Vector2f(1076.0f, 716.0f));
    border.setPosition(2.0f, 2.0f);
    border.setOutlineColor(sf::Color(200, 0, 0));
    border.setOutlineThickness(-2.0f);
    border.setFillColor(sf::Color::Transparent);
    target.draw(border);

    // Transform from level-space to screen-space
    sf::Transform transform;
    transform.translate(k_LeftBorder, target.getSize().y - k_TopBorder);
    transform.scale(Maze::k_CellSize, -Maze::k_CellSize);

    m_Maze.Draw(target, transform);
    m_Player.Draw(target, transform);
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        ghost->Draw(target, transform);
    }
}
