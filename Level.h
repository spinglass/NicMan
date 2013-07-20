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
    void UpdateBehaviour(float dt);

    std::vector<Cell> m_CellStorage;
    Grid m_Grid;
    int m_NumCols;
    int m_NumRows;

    Sprite m_Background;
    Player m_Player;
    std::vector<std::shared_ptr<Ghost>> m_Ghosts;

    std::vector<float> m_BehaviourChanges;
    int m_CurrentBehaviour;
    float m_BehaviourTimer;
};
