#pragma once

#include <vector>

class Cell;

class Grid
{
public:
    Grid();
    virtual ~Grid();

    void Initialise(int width, int height);
    void AddCell(int x, int y, Cell& cell);

    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    Cell* GetCell(int x, int y) const;

private:
    std::vector<std::vector<Cell*>> m_Cells;
    int m_Width;
    int m_Height;
};
