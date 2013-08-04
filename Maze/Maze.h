#pragma once

#include "BaseMovement.h"
#include "Grid.h"
#include "Core/Sprite.h"

class MazeRawData;

class Maze
{
public:
    Maze();
    virtual ~Maze();

    void Load(char const* name);

    void Draw(sf::RenderTarget& target, sf::Transform const& transform);

    sf::Vector2f GetPlayerStart() const { return m_PlayerStart; }
    Base const& GetBase() const { return m_Base; }
    sf::Vector2i GetBaseExit() const;

    Cell* GetCell(int x, int y) const;
    bool GetPillsRemaining() const;

    static float const k_CellSize;

private:
    Maze(Maze&);
    Maze& operator=(Maze&);

    void LoadGrid(char const* filename);
    void Parse(MazeRawData const& data);
    void BuildTunnelLinks(MazeRawData const& data);
    void BuildLinks();
    void BuildKitPartList(MazeRawData const& data);

    std::vector<Cell> m_CellStorage;
    Grid m_Grid;
    Base m_Base;
    sf::Color m_Color;
    std::string m_BaseName;
    sf::Vector2f m_PlayerStart;

    std::map<int, Sprite> m_KitParts;
    std::vector<std::vector<Sprite*>> m_CellKitParts;
    Sprite m_KitBase;
};
