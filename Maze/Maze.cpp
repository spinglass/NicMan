#include "stdafx.h"
#include "Maze.h"

#include "Core/Vector.h"
#include "Cell.h"
#include "Direction.h"
#include "Game/GlobalSettings.h"

class MazeRawData
{
public:
    MazeRawData(std::vector<char> const& data) : m_Data(data), m_Width(0), m_Height(0)
    {
        // Get width from first row
        for (int i = 0; i < data.size(); ++i)
        {
            if (data[i] == '\n')
            {
                m_Width = i;
                break;
            }
        }

        if (m_Width > 0)
        {
            // Get height and check all widths match        
            int x = 0;
            for (int i = 0; i < data.size(); ++i)
            {
                if (data[i] == '\n')
                {
                    if (x != m_Width)
                    {
                        // Mismatch
                        assert(false);
                        m_Width = 0;
                        m_Height = 0;
                        break;
                    }
                    else
                    {
                        // Next row
                        x = 0;
                        ++m_Height;
                    }
                }
                else
                {
                    // Next column
                    ++x;
                }
            }
        }
    }

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

    char Get(int x, int y) const
    {
        if (0 <= x && x < m_Width && 0 <= y && y < m_Height)
        {
            int const i = x + (m_Width + 1) * (m_Height - y - 1);
            return m_Data[i];
        }
        return ' ';
    }

    bool CanPlayerPass(int x, int y) const
    {
        char const c = Get(x, y);
        return (c == '+' || c == 'o' || c == '#' || c == '=' || ('1' <= c && c <= '9'));
    }

private:
    MazeRawData(MazeRawData&);
    MazeRawData& operator=(MazeRawData&);

    std::vector<char> const& m_Data;
    int m_Width;
    int m_Height;
};

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

            m_FruitPosition.x = mazeElement->FloatAttribute("FruitX");
            m_FruitPosition.y = mazeElement->FloatAttribute("FruitY");

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

        MazeRawData const mazeRawData(data);
        Parse(mazeRawData);
        BuildKitPartList(mazeRawData);
    }
}

void Maze::Parse(MazeRawData const& data)
{
    // Count cells
    int numCells = 0;
    for (int x = 0; x < data.Width(); ++x)
    {
        for (int y = 0; y < data.Height(); ++y)
        {
            char const c = data.Get(x, y);
            if (Cell::IsCell(c))
            {
                ++numCells;
            }
        }
    }

    if (numCells > 0)
    {
        m_CellStorage.reserve(numCells);

        // Prepare the cells
        m_Grid.Initialise(data.Width(), data.Height());

        for (int x = 0; x < data.Width(); ++x)
        {
            for (int y = 0; y < data.Height(); ++y)
            {
                char const c = data.Get(x, y);
                if (Cell::IsCell(c))
                {
                    m_CellStorage.emplace_back(c, x, y);
                    m_Grid.AddCell(x, y, m_CellStorage.back());
                }
            }
        }

        BuildLinks();
        BuildTunnelLinks(data);
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
            if (c1)
            {
                sf::Vector2i const p1(x, y);

                for (Direction dir : dirs)
                {
                    sf::Vector2i const p2 = Add(p1, 1, dir);
                    Cell* c2 = m_Grid.GetCell(p2.x, p2.y);
                    if (c2)
                    {
                        c1->SetNext(dir, c2);
                    }
                }
            }
        }
    }
}

void Maze::BuildTunnelLinks(MazeRawData const& data)
{
    struct Link
    {
        sf::Vector2i pos;
        int Id;
    };
    std::vector<Link> links;

    for (int x = 0; x < data.Width(); ++x)
    {
        for (int y = 0; y < data.Height(); ++y)
        {
            char const c = data.Get(x, y);
            if ('1' <= c && c < '9')
            {
                Link link;
                link.Id = (int)(c - '0');
                link.pos.x = x;
                link.pos.y = y;
                links.push_back(link);
            }
        }
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
                    sf::Vector2i p1 = Add(l1.pos, 1, dir);
                    sf::Vector2i p2 = Add(l2.pos, 1, opp);

                    Cell* c1 = m_Grid.GetCell(p1.x, p1.y);
                    Cell* c2 = m_Grid.GetCell(p2.x, p2.y);
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

void Maze::BuildKitPartList(MazeRawData const& data)
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
    m_CellKitParts.resize(data.Width());
    for (int x = 0; x < data.Width(); ++x)
    {
        m_CellKitParts[x].resize(data.Height());

        for (int y = 0; y < data.Height(); ++y)
        {
            // If no mark-up
            if (data.Get(x, y) == ' ')
            {
                int x1 = x;
                int y1 = y;
                int val = 0;

                // Value built based on surrounding pieces
                ++y1;
                if (data.CanPlayerPass(x1, y1)) { val |= 2; }
                ++x1;
                if (data.CanPlayerPass(x1, y1)) { val |= 4; }
                --y1;
                if (data.CanPlayerPass(x1, y1)) { val |= 8; }
                --y1;
                if (data.CanPlayerPass(x1, y1)) { val |= 16; }
                --x1;
                if (data.CanPlayerPass(x1, y1)) { val |= 32; }
                --x1;
                if (data.CanPlayerPass(x1, y1)) { val |= 64; }
                ++y1;
                if (data.CanPlayerPass(x1, y1)) { val |= 128; }
                ++y1;
                if (data.CanPlayerPass(x1, y1)) { val |= 1; }

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
                    m_CellKitParts[x][y] = &m_KitParts[foundPart];
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
    path.setFillColor(sf::Color(0, 25, 0));

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
