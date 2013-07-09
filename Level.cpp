#include "stdafx.h"
#include "Level.h"

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

    if (m_BGTexture.loadFromFile("Resources/level01_bg.png"))
    {
        m_BGSprite.setTexture(m_BGTexture, true);
    }
}

void Level::Parse(std::vector<char> const& data)
{
    int numCols = 0;
    int numRows = 0;

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
        // Get number of rows and check all lengths
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
                ++rowPos;
            }
        }
    }

    if (numCols > 0 && numRows > 0)
    {
        // Prepare the cells
        m_Cells.resize(numCols);
        for (int col = 0; col < numCols; ++col)
        {
            m_Cells[col].resize(numRows);
        }

        int i = 0;
        for (int row = 0; row < numRows; ++row)
        {
            for (int col = 0; col < numCols; ++col)
            {
                m_Cells[col][row] = Cell::Parse(data[i]);
                ++i;
            }
            ++i;
        }
    }

    m_NumCols = numCols;
    m_NumRows = numRows;
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

    m_BGSprite.setPosition(k_LeftBorder, k_TopBorder);
    target.draw(m_BGSprite);

    bool const k_ShowWall = false;
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

    bool const k_ShowPath = false;
    sf::RectangleShape path(sf::Vector2f(k_CellSize, k_CellSize));
    path.setOrigin(0.5f * path.getSize());
    path.setOutlineThickness(-1.0f);
    path.setOutlineColor(sf::Color(50, 50, 50));
    path.setFillColor(sf::Color(25, 25, 25));

    for (int col = 0; col < m_NumCols; ++col)
    {
        for (int row = 0; row < m_NumRows; ++row)
        {
            Cell const& cell = m_Cells[col][row];
            switch(cell.GetType())
            {
            default:
                break;
            case Cell::Space:
            case Cell::GhostBase:
                if (k_ShowPath)
                {
                    path.setPosition(k_LeftBorder + (col + 0.5f) * k_CellSize, k_TopBorder + (row + 0.5f) * k_CellSize);
                    target.draw(path);
                }
                if (cell.GetPill())
                {
                    pill.setPosition(k_LeftBorder + (col + 0.5f) * k_CellSize, k_TopBorder + (row + 0.5f) * k_CellSize);
                    target.draw(pill);
                }
                else if (cell.GetPowerPill())
                {
                    powerPill.setPosition(k_LeftBorder + (col + 0.5f) * k_CellSize, k_TopBorder + (row + 0.5f) * k_CellSize);
                    target.draw(powerPill);
                }
                break;
            case Cell::Wall:
                if (k_ShowWall)
                {
                    wall.setPosition(k_LeftBorder + (col + 0.25f) * k_CellSize, k_TopBorder + (row + 0.25f) * k_CellSize);
                    target.draw(wall);
                    wall.setPosition(k_LeftBorder + (col + 0.25f) * k_CellSize, k_TopBorder + (row + 0.75f) * k_CellSize);
                    target.draw(wall);
                    wall.setPosition(k_LeftBorder + (col + 0.75f) * k_CellSize, k_TopBorder + (row + 0.25f) * k_CellSize);
                    target.draw(wall);
                    wall.setPosition(k_LeftBorder + (col + 0.75f) * k_CellSize, k_TopBorder + (row + 0.75f) * k_CellSize);
                    target.draw(wall);
                    break;
                }
            }
        }
    }
}
