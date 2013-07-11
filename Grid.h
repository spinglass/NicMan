#pragma once

#include <vector>

class Cell;

class Grid
{
public:
    Grid();
    virtual ~Grid();

    void Initialise(int cols, int rows);
    void AddCell(int col, int row, Cell const& cell);

    Cell const* GetCell(int col, int row) const;

private:
    std::vector<std::vector<Cell const*>> m_Cells;
};
