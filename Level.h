#pragma once

#include "Cell.h"
#include "Sprite.h"
#include "Entities/Player.h"

class Level
{
public:
    Level();
    virtual ~Level();

    void Load(char* filename);

    void Draw(sf::RenderTarget& target);

private:
    void Parse(std::vector<char> const& data);

    std::vector<std::vector<Cell>> m_Cells;
    int m_NumCols;
    int m_NumRows;

    Sprite m_Background;
    Player m_Player;
};
