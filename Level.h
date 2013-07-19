#pragma once

#include "Sprite.h"
#include "Entities/Ghost.h"
#include "Entities/Player.h"
#include "Maze/Grid.h"

class Level
{
public:
    Level();
    virtual ~Level();

    void Load(char* filename);

    void Update(float dt);
    void Draw(sf::RenderTarget& target);

private:
    void Parse(std::vector<char> const& data);

    std::vector<Cell> m_CellStorage;
    Grid m_Grid;
    int m_NumCols;
    int m_NumRows;

    Sprite m_Background;
    Player m_Player;
    std::vector<Ghost> m_Ghosts;
};
