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
    FILE* file;
    if (0 == fopen_s(&file, filename, "r"))
    {
        // Get file size
        fseek(file, 0, SEEK_END);
        fflush(file);
        int size = (int)ftell(file);
        fseek(file, 0, SEEK_SET);

        // Read it
        std::vector<char> data(size);
        fread_s(data.data(), size, sizeof(char), size, file);
        fclose(file);

        Parse(data);
    }

    m_Background.Load("Resources/level01_bg");

    m_Player.Load();
    m_Player.SetPosition(GridRef(&m_Grid, 14, 7), 0.0f, 0.5f);

    m_Ghosts.resize(4);
    for (int i = 0; i < m_Ghosts.size(); ++i)
    {
        m_Ghosts[i] = std::make_shared<Ghost>();
        m_Ghosts[i]->Load(i);
    }

    std::shared_ptr<IGhostTarget> eatenTarget = std::make_shared<FixedTarget>(GridRef(&m_Grid, 13, 19));
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<BlinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(GridRef(&m_Grid, 25, 32));

        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[0]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[0]->SetPosition(GridRef(&m_Grid, 13, 19), 1.0f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<PinkyTarget>(m_Player.GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(GridRef(&m_Grid, 2, 32));

        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[1]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[1]->SetPosition(GridRef(&m_Grid, 4, 22), 0.1f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<InkyTarget>(m_Player.GetMovement(), m_Ghosts[0]->GetMovement());
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(GridRef(&m_Grid, 27, -1));

        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[2]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[2]->SetPosition(GridRef(&m_Grid, 24, 22), 0.2f, 0.5f);
    }
    {
        std::shared_ptr<IGhostTarget> chaseTarget = std::make_shared<ClydeTarget>(m_Player.GetMovement(), m_Ghosts[3]->GetMovement(), GridRef(&m_Grid, 0, -1));
        std::shared_ptr<IGhostTarget> scatterTarget = std::make_shared<FixedTarget>(GridRef(&m_Grid, 0, -1));

        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Chase, chaseTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Scatter, scatterTarget);
        m_Ghosts[3]->SetTarget(Ghost::Behaviour::Eaten, eatenTarget);
        m_Ghosts[3]->SetPosition(GridRef(&m_Grid, 14, 25), 0.3f, 0.5f);
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

void Level::Parse(std::vector<char> const& data)
{
    int numCols = 0;
    int numRows = 0;
    int numCells = 0;

    // Get row length
    for (int i = 0; i < data.size(); ++i)
    {
        if (data[i] == '\n')
        {
            numCols = i;
            break;
        }
    }

    if (numCols > 0)
    {
        // Get number of rows and check all lengths and count cells        
        int rowPos = 0;
        for (int i = 0; i < data.size(); ++i)
        {
            if (data[i] == '\n')
            {
                if (rowPos != numCols)
                {
                    numRows = 0;
                    break;
                }
                else
                {
                    rowPos = 0;
                    ++numRows;
                }
            }
            else
            {
                if (Cell::IsCell(data[i]))
                {
                    ++numCells;
                }
                ++rowPos;
            }
        }
    }

    if (numCols > 0 && numRows > 0 && numCells > 0)
    {
        m_CellStorage.reserve(numCells);

        // Prepare the cells
        m_Grid.Initialise(numCols, numRows);

        int i = 0;
        // Not reverse iteration through rows, to get [0,0] to the bottom-left
        for (int row = numRows - 1; row >= 0; --row)
        {
            for (int col = 0; col < numCols; ++col)
            {
                if (Cell::IsCell(data[i]))
                {
                    m_CellStorage.emplace_back(data[i]);
                    m_Grid.AddCell(col, row, m_CellStorage.back());
                }
                ++i;
            }
            ++i;
        }
    }

    m_NumCols = numCols;
    m_NumRows = numRows;
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
    float const k_CellSize = 20.0f;
    sf::Vector2f const k_HalfCell(0.5f * k_CellSize, 0.5f * k_CellSize);

    sf::RectangleShape border(sf::Vector2f(1076.0f, 716.0f));
    border.setPosition(2.0f, 2.0f);
    border.setOutlineColor(sf::Color(200, 0, 0));
    border.setOutlineThickness(-2.0f);
    border.setFillColor(sf::Color::Transparent);
    target.draw(border);

    m_Background.SetPosition(k_LeftBorder, k_TopBorder);
    m_Background.Draw(target);

    static bool k_ShowWall = false;
    sf::RectangleShape wall(k_HalfCell);
    wall.setOrigin(0.5f * wall.getSize());
    wall.setOutlineColor(sf::Color(0, 0, 50));
    wall.setOutlineThickness(-1.0f);
    wall.setFillColor(sf::Color(0, 0, 0));

    float const k_PillSize = 6.0f;
    sf::RectangleShape pill(sf::Vector2f(k_PillSize, k_PillSize));
    pill.setOrigin(0.5f * pill.getSize());
    pill.setFillColor(sf::Color::White);

    float const k_PowerPillSize = 8.0f;
    sf::CircleShape powerPill(k_PowerPillSize);
    powerPill.setOrigin(sf::Vector2f(k_PowerPillSize, k_PowerPillSize));
    powerPill.setFillColor(sf::Color::White);

    static bool k_ShowCell = false;
    sf::RectangleShape path(sf::Vector2f(k_CellSize, k_CellSize));
    path.setOrigin(0.5f * path.getSize());
    path.setOutlineThickness(-1.0f);
    path.setOutlineColor(sf::Color(50, 50, 50));
    path.setFillColor(sf::Color(25, 25, 25));

    // Transform from level-space to screen-space
    sf::Transform transform;
    transform.translate(k_LeftBorder, target.getSize().y - k_TopBorder);
    transform.scale(k_CellSize, -k_CellSize);

    for (int col = 0; col < m_NumCols; ++col)
    {
        float const colCentre = col + 0.5f;

        for (int row = 0; row < m_NumRows; ++row)
        {
            float const rowCentre = row + 0.5f;
            sf::Vector2f const cellPosition = transform.transformPoint(colCentre, rowCentre);

            if (Cell const* cell = m_Grid.GetCell(col, row))
            {
                if (k_ShowCell)
                {
                    path.setPosition(cellPosition);
                    target.draw(path);
                }
                if (cell->GetPill())
                {
                    pill.setPosition(cellPosition);
                    target.draw(pill);
                }
                else if (cell->GetPowerPill())
                {
                    powerPill.setPosition(cellPosition);
                    target.draw(powerPill);
                }
            }
            else if (k_ShowWall)
            {
                wall.setPosition(cellPosition + sf::Vector2f(-0.25f * k_CellSize, -0.25f * k_CellSize));
                target.draw(wall);
                wall.setPosition(cellPosition + sf::Vector2f(-0.25f * k_CellSize, 0.25f * k_CellSize));
                target.draw(wall);
                wall.setPosition(cellPosition + sf::Vector2f(0.25f * k_CellSize, -0.25f * k_CellSize));
                target.draw(wall);
                wall.setPosition(cellPosition + sf::Vector2f(0.25f * k_CellSize, 0.25f * k_CellSize));
                target.draw(wall);
            }
        }
    }

    m_Player.Draw(target, transform);
    for (std::shared_ptr<Ghost>& ghost : m_Ghosts)
    {
        ghost->Draw(target, transform);
    }
}
