#include "stdafx.h"
#include "Maze.h"

#include "Core/Vector.h"
#include "Cell.h"
#include "Direction.h"
#include "Game/GlobalSettings.h"

float const Maze::k_CellSize = 20.0f;

Maze::Maze() :
    m_BaseName("Mazes/KitBase")
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
            char const* colorHex = mazeElement->Attribute("Color");
            int color = 0;
            sscanf_s(colorHex, "%x", &color);
            m_Color = sf::Color((sf::Uint8)(color >> 16), (sf::Uint8)(color >> 8), (sf::Uint8)color);

            if (char const* baseName = mazeElement->Attribute("Base"))
            {
                m_BaseName = baseName;
            }

            char const* grid = mazeElement->Attribute("Grid");
            assert(grid);
            LoadGrid(grid);

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
        BuildKitPartList();
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
                    m_CellStorage.emplace_back(data[i], col, row);
                    m_Grid.AddCell(col, row, m_CellStorage.back());
                }
                ++i;
            }
            ++i;
        }

        BuildLinks();
        BuildTunnelLinks(data, numRows, numCols);
    }
}

void Maze::BuildLinks()
{
    Direction const dirs[] = { Direction::North, Direction::East, Direction::South, Direction::West };

    for (int x = 0; x < m_Grid.GetWidth(); ++x)
    {
        for (int y = 0; y < m_Grid.GetHeight(); ++y)
        {
            Cell* c1 = m_Grid.GetCell(x, y);
            if (c1 && c1->IsOpen())
            {
                sf::Vector2i const p1(x, y);

                for (Direction dir : dirs)
                {
                    sf::Vector2i const p2 = Add(p1, 1, dir);
                    Cell* c2 = m_Grid.GetCell(p2.x, p2.y);
                    if (c2 && c2->IsOpen())
                    {
                        c1->SetNext(dir, c2);
                    }
                }
            }
        }
    }
}

void Maze::BuildTunnelLinks(std::vector<char> const& data, int numRows, int numCols)
{
    struct Link
    {
        sf::Vector2i pos;
        int Id;
    };
    std::vector<Link> links;

    int i = 0;
    for (int row = numRows - 1; row >= 0; --row)
    {
        for (int col = 0; col < numCols; ++col)
        {
            if ('1' <= data[i] && data[i] < '9')
            {
                Link link;
                link.Id = (int)(data[i] - '0');
                link.pos.x = col;
                link.pos.y = row;
                links.push_back(link);
            }
            ++i;
        }
        ++i;
    }

    // Apply links to cells
    Direction const dirs[] = { Direction::North, Direction::East, Direction::South, Direction::West };
    for (int i = 0; i < links.size(); ++i)
    {
        Link const& l1 = links[i];

        for (int j = i + 1; j < links.size(); ++j)
        {
            Link const& l2 = links[j];
            if (l1.Id == l2.Id)
            {

                for (Direction dir : dirs)
                {
                    Direction opp = Opposite(dir);

                    Cell* c1 = m_Grid.GetCell(l1.pos.x, l1.pos.y);
                    Cell* c2 = m_Grid.GetCell(l2.pos.x, l2.pos.y);
                    if (c1 && c2)
                    {
                        c1->SetNext(opp, c2);
                        c2->SetNext(dir, c1);
                    }
                }

                break;
            }
        }
    }
}

void Maze::BuildKitPartList()
{
    // Avialable kit parts.
    // Array ordered to find in correct order with bitwise compare.
    int kitParts[] = { 10, 40, 130, 160, 2, 8, 32, 128, 1, 4, 16, 64 };

    // Load the parts
    char filename[FILENAME_MAX];
    for (int part : kitParts)
    {
        sprintf_s(filename, "Mazes/Kit%03d", part);
        m_KitParts[part].Load(filename);
        m_KitParts[part].SetOriginToCentre();
        m_KitParts[part].SetColor(m_Color);
    }
    m_KitBase.Load(m_BaseName.c_str());
    m_KitBase.SetOriginToCentre();
    m_KitBase.SetColor(m_Color);

    // Map the cells onto the correct parts
    m_CellKitParts.resize(m_Grid.GetWidth());
    for (int col = 0; col < m_Grid.GetWidth(); ++col)
    {
        m_CellKitParts[col].resize(m_Grid.GetHeight());

        for (int row = 0; row < m_Grid.GetHeight(); ++row)
        {
            Cell* cell = m_Grid.GetCell(col, row);
            if (!cell)
            {
                int x = col;
                int y = row;
                int val = 0;

                // Value built based on surrounding pieces
                ++y;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 2; }
                ++x;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 4; }
                --y;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 8; }
                --y;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 16; }
                --x;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 32; }
                --x;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 64; }
                ++y;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 128; }
                ++y;
                cell = m_Grid.GetCell(x, y);
                if (cell && cell->IsOpen()) { val |= 1; }

                if (val != 0)
                {
                    int foundPart = 0;
                    for (int part : kitParts)
                    {
                        if ((val & part) == part)
                        {
                            foundPart = part;
                            break;
                        }
                    }
                    assert(foundPart);
                    m_CellKitParts[col][row] = &m_KitParts[foundPart];
                }
            }
        }
    }
}

void Maze::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f const k_HalfCell(0.5f * k_CellSize, 0.5f * k_CellSize);

    // Ghost base
    sf::Vector2f const basePosition = transform.transformPoint(m_Base.ExitX, m_Base.Middle);
    m_KitBase.SetPosition(basePosition);
    m_KitBase.Draw(target);

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

    for (int col = 0; col < m_Grid.GetWidth(); ++col)
    {
        float const colCentre = col + 0.5f;

        for (int row = 0; row < m_Grid.GetHeight(); ++row)
        {
            float const rowCentre = row + 0.5f;
            sf::Vector2f const cellPosition = transform.transformPoint(colCentre, rowCentre);

            // Render pill or power pill for cell
            if (Cell const* cell = m_Grid.GetCell(col, row))
            {
                if (GlobalSettings::It().DebugCells)
                {
                    if (cell->IsOpen())
                    {
                        path.setFillColor(sf::Color(0, 25, 0));
                    }
                    else
                    {
                        path.setFillColor(sf::Color(25, 0, 0));
                    }

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
            // Render kit part for cell, if there is one
            else if (Sprite* sprite = m_CellKitParts[col][row])
            {
                sprite->SetPosition(cellPosition);
                sprite->Draw(target);
            }
        }
    }
}

Cell* Maze::GetCell(int x, int y) const
{
    return m_Grid.GetCell(x, y);
}

sf::Vector2i Maze::GetBaseExit() const
{
    return sf::Vector2i((int)m_Base.ExitX, (int)m_Base.ExitY);
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
