#pragma once

#include <vector>

#include "GridRef.h"

class Cell;

class Grid
{
public:
    Grid();
    virtual ~Grid();

    void Initialise(int width, int height);
    void AddCell(int x, int y, Cell& cell);
    void BuildWarpTargets();

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    Cell* GetCell(int x, int y) const;
    GridRef GetWarpTarget(Cell const* cell) const;

private:
    std::vector<std::vector<Cell*>> m_Cells;
    std::map<Cell const*, GridRef> m_WarpTarget;
    int m_Width;
    int m_Height;
};
