#include "stdafx.h"
#include "Maze.h"

#include "Cell.h"
#include "Game/GlobalSettings.h"

float const Maze::k_CellSize = 20.0f;

Maze::Maze()
{
}

Maze::~Maze()
{
}

void Maze::Load(char const* filename)
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile(filename) == tinyxml2::XML_NO_ERROR)
    {
        if (tinyxml2::XMLElement* mazeElement = doc.FirstChildElement("Maze"))
        {
            char const* grid = mazeElement->Attribute("Grid");
            assert(grid);
            LoadGrid(grid);

            char const* bg = mazeElement->Attribute("Background");
            assert(bg);
            m_Background.Load(bg);

            m_PlayerStart.x = mazeElement->FloatAttribute("PlayerX");
            m_PlayerStart.y = mazeElement->FloatAttribute("PlayerY");

            tinyxml2::XMLElement* baseElement = mazeElement->FirstChildElement("Base");
            assert(baseElement);
            m_Base.Load(*baseElement);
        }
    }
}

void Maze::LoadGrid(char const* filename)
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
}

void Maze::Parse(std::vector<char> const& data)
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
}

void Maze::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f const k_HalfCell(0.5f * k_CellSize, 0.5f * k_CellSize);

    sf::Vector2f const bgPos = transform.transformPoint(0.0f, (float)m_Grid.GetHeight());
    m_Background.SetPosition(bgPos);
    m_Background.Draw(target);

    float const k_PillSize = 6.0f;
    sf::RectangleShape pill(sf::Vector2f(k_PillSize, k_PillSize));
    pill.setOrigin(0.5f * pill.getSize());
    pill.setFillColor(sf::Color::White);

    float const k_PowerPillSize = 8.0f;
    sf::CircleShape powerPill(k_PowerPillSize);
    powerPill.setOrigin(sf::Vector2f(k_PowerPillSize, k_PowerPillSize));
    powerPill.setFillColor(sf::Color::White);

    sf::RectangleShape path(sf::Vector2f(k_CellSize, k_CellSize));
    path.setOrigin(0.5f * path.getSize());
    path.setOutlineThickness(-1.0f);
    path.setOutlineColor(sf::Color(50, 50, 50));
    path.setFillColor(sf::Color(25, 25, 25));

    for (int col = 0; col < m_Grid.GetWidth(); ++col)
    {
        float const colCentre = col + 0.5f;

        for (int row = 0; row < m_Grid.GetHeight(); ++row)
        {
            float const rowCentre = row + 0.5f;
            sf::Vector2f const cellPosition = transform.transformPoint(colCentre, rowCentre);

            if (Cell const* cell = m_Grid.GetCell(col, row))
            {
                if (GlobalSettings::It().DebugCells)
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
        }
    }
}

GridRef Maze::GetGridRef(int x, int y) const
{
    return GridRef(&m_Grid, x, y);
}

bool Maze::GetPillsRemaining() const
{
    bool pillFound = false;
    for (Cell cell : m_CellStorage)
    {
        if (cell.GetPill() || cell.GetPowerPill())
        {
            pillFound = true;
            break;
        }
    }
    return pillFound;
}
