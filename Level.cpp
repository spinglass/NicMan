#include "stdafx.h"
#include "Level.h"

#include "Maze/Cell.h"
#include "GhostTargets/BlinkyTarget.h"
#include "GhostTargets/PinkyTarget.h"
#include "GhostTargets/InkyTarget.h"
#include "GhostTargets/ClydeTarget.h"
#include "GhostTargets/FixedTarget.h"

Level::Level() :
    m_BehaviourCounter(0),
    m_BehaviourTimer(0.0f),
    m_MainBehaviour(Ghost::Behaviour::Scatter),
    m_PowerTimer(0.0f)
{
}

Level::~Level()
{
}

void Level::Load(char* filename)
{
    m_Maze.Load(filename);

    m_Player.Load();
    m_Player.SetPosition(m_Maze.GetGridRef(14, 7), 0.0f, 0.5f);

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
        m_Ghosts[0]->SetPosition(m_Maze.GetGridRef(13, 19), 1.0f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<PinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(2, 32));

        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[1]->SetPosition(m_Maze.GetGridRef(4, 22), 0.1f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<InkyTarget>(m_Player.GetMovement(), m_Ghosts[0]->GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef(27, -1));

        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[2]->SetPosition(m_Maze.GetGridRef(24, 22), 0.2f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<ClydeTarget>(m_Player.GetMovement(), m_Ghosts[3]->GetMovement(), m_Maze.GetGridRef(0, -1));
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(m_Maze.GetGridRef( 0, -1));

        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[3]->SetPosition(m_Maze.GetGridRef(14, 25), 0.3f, 0.5f);
    }

    // Setup chase/scatter cycle
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(7.0f);
    m_BehaviourChanges.push_back(20.0f);
    m_BehaviourChanges.push_back(5.0f);
}

void Level::Update(float dt)
{
    UpdateBehaviour(dt);
    UpdatePowerPlay(dt);

    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        ghost->Update(dt);
    }
    m_Player.Update(dt);
}

void Level::UpdateBehaviour(float dt)
{
    if (m_PowerTimer > 0.0f)
    {
        if (m_PowerTimer < 1.0f)
        {
            // Flash
            bool const frightFlash = (int)floorf(4.0f * m_PowerTimer) % 2 == 1;

            // Apply to all ghosts
            for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
            {
                ghost->SetFrightFlash(frightFlash);
            }
        }
    }
    else 
    {
        if (m_BehaviourCounter < m_BehaviourChanges.size())
        {
            m_BehaviourTimer += dt;
            if (m_BehaviourTimer > m_BehaviourChanges[m_BehaviourCounter])
            {
                // Swap behaviour
                m_MainBehaviour = (m_MainBehaviour == Ghost::Behaviour::Chase) ? Ghost::Behaviour::Scatter : Ghost::Behaviour::Chase;

                // Reset for next change
                m_BehaviourTimer = 0.0f;
                ++m_BehaviourCounter;
            }
        }

        // Apply to all ghosts
        for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
        {
            ghost->SetBehaviour(m_MainBehaviour);
        }
    }
}

void Level::UpdatePowerPlay(float dt)
{
    if (m_Player.AtePowerPill())
    {
        m_PowerTimer = 6.0f;

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
    else if (m_PowerTimer > 0.0f)
    {
        m_PowerTimer -= dt;

        for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
        {
            if (m_Player.GetMovement().GetPosition() == ghost->GetMovement().GetPosition())
            {
                // Eat ghost!
                ghost->SetBehaviour(Ghost::Behaviour::Eaten);
            }
        }
    }
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
