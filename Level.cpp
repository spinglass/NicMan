#include "stdafx.h"
#include "Level.h"

#include "Maze/Cell.h"
#include "GhostTargets/BlinkyTarget.h"
#include "GhostTargets/PinkyTarget.h"
#include "GhostTargets/InkyTarget.h"
#include "GhostTargets/ClydeTarget.h"

Level::Level()
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
        m_Ghosts[i].Load(i);
    }

    m_Ghosts[0].SetTarget(std::make_shared<BlinkyTarget>(m_Player.GetPosition()));
    m_Ghosts[0].SetPosition(GridRef(&m_Grid, 14, 19), 0.0f, 0.5f);

    m_Ghosts[1].SetTarget(std::make_shared<PinkyTarget>(m_Player.GetPosition(), m_Player.GetDirection()));
    m_Ghosts[1].SetPosition(GridRef(&m_Grid, 2, 22), 0.0f, 0.5f);

    m_Ghosts[2].SetTarget(std::make_shared<InkyTarget>(m_Player.GetPosition(), m_Player.GetDirection(), m_Ghosts[0].GetPosition()));
    m_Ghosts[2].SetPosition(GridRef(&m_Grid, 26, 22), 0.0f, 0.5f);

    m_Ghosts[3].SetTarget(std::make_shared<ClydeTarget>(m_Player.GetPosition(), m_Ghosts[3].GetPosition(), GridRef(&m_Grid, 1, -2)));
    m_Ghosts[3].SetPosition(GridRef(&m_Grid, 14, 25), 0.0f, 0.5f);
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
    for (Ghost& ghost : m_Ghosts)
    {
        ghost.Update(dt);
    }
    m_Player.Update(dt);
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
    for (Ghost& ghost : m_Ghosts)
    {
        ghost.Draw(target, transform);
    }
}
