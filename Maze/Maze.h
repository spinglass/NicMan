#pragma once

#include "BaseMovement.h"
#include "Grid.h"
#include "GridRef.h"
#include "Core/Sprite.h"

class Maze
{
public:
    Maze();
    virtual ~Maze();

    void Load(char const* name);

    void Draw(sf::RenderTarget& target, sf::Transform const& transform);

    sf::Vector2f GetPlayerStart() const { return m_PlayerStart; }
    Base const& GetBase() const { return m_Base; }

    GridRef GetGridRef(int x, int y) const;
    bool GetPillsRemaining() const;

    static float const k_CellSize;

private:
    Maze(Maze&);
    Maze& operator=(Maze&);

    void LoadGrid(char const* filename);
    void Parse(std::vector<char> const& data);
    void BuildKitPartList();

    std::vector<Cell> m_CellStorage;
    Grid m_Grid;
    Base m_Base;
    sf::Color m_Color;
    sf::Vector2f m_PlayerStart;

    std::map<int, Sprite> m_KitParts;
    std::vector<std::vector<Sprite*>> m_CellKitParts;
    Sprite m_KitBase;
};
