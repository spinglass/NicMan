#pragma once

#include <vector>

class Cell;

class Grid
{
public:
    Grid();
    virtual ~Grid();

    void Initialise(int width, int height);
    void AddCell(int x, int y, Cell const& cell);

    Cell const* GetCell(int x, int y) const;

private:
    std::vector<std::vector<Cell const*>> m_Cells;
    int m_Width;
    int m_Height;
};
