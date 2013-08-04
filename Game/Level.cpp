#include "stdafx.h"
#include "Level.h"

#include "GhostTargets/BlinkyTarget.h"
#include "GhostTargets/PinkyTarget.h"
#include "GhostTargets/InkyTarget.h"
#include "GhostTargets/ClydeTarget.h"
#include "GhostTargets/FixedTarget.h"
#include "GlobalSettings.h"
#include "Maze/Cell.h"
#include "ScoreManager.h"

struct LevelSettings
{
    LevelSettings(tinyxml2::XMLElement const& element)
    {
        FrightTime = element.FloatAttribute("FrightTime");
        FrightFlashTime = element.FloatAttribute("FrightFlashTime");
        PlayerNormSpeedFactor = element.FloatAttribute("PlayerNormSpeedFactor");
        PlayerFrightSpeedFactor = element.FloatAttribute("PlayerFrightSpeedFactor");
        GhostNormSpeedFactor = element.FloatAttribute("GhostNormSpeedFactor");
        GhostFrightSpeedFactor = element.FloatAttribute("GhostFrightSpeedFactor");
        GhostTunnelSpeedFactor = element.FloatAttribute("GhostTunnelSpeedFactor");
        InkyExitTime = element.FloatAttribute("InkyExitTime");
        ClydeExitTime = element.FloatAttribute("ClydeExitTime");
    }

    float FrightTime;
    float FrightFlashTime;
    float PlayerNormSpeedFactor;
    float PlayerFrightSpeedFactor;
    float GhostNormSpeedFactor;
    float GhostFrightSpeedFactor;
    float GhostTunnelSpeedFactor;
    float InkyExitTime;
    float ClydeExitTime;
};

float const Level::s_MaxSpeed = 8.0f;
float const Level::s_StartWait = 2.0f;
float const Level::s_DeathWait = 2.0f;
float const Level::s_CompletionWait = 2.0f;

Level::Level(ScoreManager& scoreManager) :
    m_ScoreManager(scoreManager),
    m_Player(m_Maze),
    m_BehaviourCounter(0),
    m_BehaviourTimer(0.0f),
    m_State(State::Start),
    m_WaitTimer(0.0f),
    m_FrightTimer(0.0f),
    m_Complete(false),
    m_NormalBehaviour(Ghost::Behaviour::Scatter),
    m_ScorePill(10),
    m_ScorePowerPill(50),
    m_GhostEatCount(0),
    m_EatenGhost(nullptr)
{
}

Level::~Level()
{
}

void Level::Load(char* filename)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename) == tinyxml2::XML_NO_ERROR)
    {
        tinyxml2::XMLElement* levelElement = doc.FirstChildElement("Level");
        assert(levelElement);

        {
            // Load maze
            tinyxml2::XMLElement* element = levelElement->FirstChildElement("Maze");
            assert(element);
            char const* definition = element->Attribute("Definition");
            assert(definition);
            m_Maze.Load(definition);
        }

        {
            // Load fruit
            if (tinyxml2::XMLElement* element = levelElement->FirstChildElement("Fruit"))
            {
                m_Fruit.Load(*element);
                m_Fruit.SetPosition(m_Maze.GetFruitPosition());
            }
        }

        {
            // Read settings
            tinyxml2::XMLElement* element = levelElement->FirstChildElement("Settings");
            assert(element);
            m_Settings = std::make_shared<LevelSettings>(*element);
        }

        {
            // Read Ghost scores
            tinyxml2::XMLElement* element = levelElement->FirstChildElement("GhostScores");
            assert(element);

            tinyxml2::XMLElement* child = element->FirstChildElement("GhostScore");
            while (child)
            {
                int const value = child->IntAttribute("Value");
                char const* sprite = child->Attribute("Sprite");
                assert(sprite);

                GhostScore ghostScore;
                ghostScore.Value = value;
                ghostScore.Sprite = std::make_shared<Sprite>();
                ghostScore.Sprite->Load(sprite);
                ghostScore.Sprite->SetOriginToCentre();

                m_GhostScores.push_back(ghostScore);

                child = child->NextSiblingElement("GhostScore");
            }
        }

        {
            // Setup chase/scatter cycle
            tinyxml2::XMLElement* element = levelElement->FirstChildElement("BehaviourChanges");
            assert(element);

            tinyxml2::XMLElement* child = element->FirstChildElement("BehaviourChange");
            while (child)
            {
                float const time = child->FloatAttribute("Time");
                m_BehaviourChanges.push_back(time);

                child = child->NextSiblingElement("BehaviourChange");
            }
        }
    }

    m_Player.Load();

    m_Ghosts.resize(4);
    for (int i = 0; i < m_Ghosts.size(); ++i)
    {
        m_Ghosts[i] = std::make_shared<Ghost>(m_Maze);
        m_Ghosts[i]->Load(i);
    }

    std::shared_ptr<IGhostTarget> eatenTarget = std::make_shared<FixedTarget>(m_Maze.GetBaseExit());
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<BlinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(sf::Vector2i(25, 32));

        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[0]->SetHomePosition(BaseMovement::HomePosition::Middle);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<PinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(sf::Vector2i(2, 32));

        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[1]->SetHomePosition(BaseMovement::HomePosition::Middle);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<InkyTarget>(m_Player.GetMovement(), m_Ghosts[0]->GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(sf::Vector2i(27, -1));

        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[2]->SetHomePosition(BaseMovement::HomePosition::Left);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<ClydeTarget>(m_Player.GetMovement(), m_Ghosts[3]->GetMovement(), sf::Vector2i(0, -1));
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(sf::Vector2i( 0, -1));

        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[3]->SetHomePosition(BaseMovement::HomePosition::Right);
    }

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
    if (m_FrightTimer < m_Settings->FrightFlashTime)
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
            m_ScoreManager.Add(m_GhostScores[m_GhostEatCount].Value);

            // Switch to eat state for a short while
            m_State = State::Eat;
            m_WaitTimer = 1.0f;

            // Ensure we only eat one ghost per frame
            m_EatenGhost = ghost.get();
            break;
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

        // Prepare for next ghost
        m_GhostEatCount = std::min(m_GhostEatCount + 1, (int)m_GhostScores.size() - 1);
        m_EatenGhost = nullptr;
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
        float ghostSpeedFactor = m_Settings->GhostNormSpeedFactor;
        if (ghost->GetMovement().GetCell() && ghost->GetMovement().GetCell()->IsTunnel())
        {
            ghostSpeedFactor = m_Settings->GhostTunnelSpeedFactor;
        }
        else if (ghost->GetBehaviour() == Ghost::Behaviour::Fright)
        {
            ghostSpeedFactor = m_Settings->GhostFrightSpeedFactor;
        }
        ghost->SetSpeed(ghostSpeedFactor * s_MaxSpeed);
        ghost->Update(dt);
    }

    float const playerSpeedFactor = (m_State == State::Fright) ? m_Settings->PlayerFrightSpeedFactor : m_Settings->PlayerNormSpeedFactor;
    m_Player.SetSpeed(playerSpeedFactor * s_MaxSpeed);
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
        m_FrightTimer = m_Settings->FrightTime;

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
        m_Fruit.NomPill();
    }

    if (m_Player.AtePowerPill() || m_Player.AtePill())
    {
        if (!m_Maze.GetPillsRemaining())
        {
            // Level finished!
            m_State = State::Complete;
            m_WaitTimer = s_CompletionWait;
        }
    }

    // Check for player eating fruit
    m_Fruit.Update(dt);
    if (m_Fruit.IsVisible())
    {
        if (m_Fruit.GetPosition() == m_Player.GetMovement().GetPosition())
        {
            m_Fruit.Nom();
            m_ScoreManager.Add(m_Fruit.GetScore());
        }
    }


    // Check for ghosts eating player
    if (!GlobalSettings::It().DebugImmortal)
    {
        for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
        {
            if ((ghost->GetBehaviour() == Ghost::Behaviour::Chase || ghost->GetBehaviour() == Ghost::Behaviour::Scatter) &&
                m_Player.GetMovement().GetPosition() == ghost->GetMovement().GetPosition())
            {
                m_State = State::Death;
                m_WaitTimer = s_DeathWait;
            }
        }
    }
}

void Level::Restart()
{
    m_State = State::Start;
    m_BehaviourCounter = 0;
    m_BehaviourTimer = 0.0f;
    m_WaitTimer = s_StartWait;
    m_NormalBehaviour = Ghost::Behaviour::Scatter;
    m_FrightTimer = 0.0f;

    sf::Vector2f playerStart = m_Maze.GetPlayerStart();
    m_Player.Restart(playerStart.x, playerStart.y);

    // Start Blinky at maze exit. Nudge left to force route on standard maze.
    m_Ghosts[0]->Restart(m_Maze.GetBase().ExitX - 0.01f, m_Maze.GetBase().ExitY);

    m_Ghosts[1]->RestartInBase();
    m_Ghosts[1]->SetExitTime(0.0f);

    m_Ghosts[2]->RestartInBase();
    m_Ghosts[2]->SetExitTime(m_Settings->InkyExitTime);

    m_Ghosts[3]->RestartInBase();
    m_Ghosts[3]->SetExitTime(m_Settings->ClydeExitTime);
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
    m_Fruit.Draw(target, transform);

    if (m_State == State::Eat)
    {
        Sprite& sprite = *(m_GhostScores[m_GhostEatCount].Sprite);
        sf::Vector2f const gridPos = m_Player.GetMovement().GetAbsolutePosition();
        sf::Vector2f const pos = transform.transformPoint(gridPos);
        sprite.SetPosition(pos);
        sprite.Draw(target);
    }
    else
    {
        m_Player.Draw(target, transform);
    }

    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        // Don't draw eaten ghosts (will only be while displaying the score)
        if (ghost.get() != m_EatenGhost)
        {
            ghost->Draw(target, transform);
        }
    }
}
