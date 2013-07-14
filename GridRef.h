#pragma once

class Cell;
class Grid;

class GridRef
{
public:
    GridRef();
    GridRef(Grid const* grid, int x, int y);
    virtual ~GridRef();

    operator bool() const;
    bool CanPlayerPass() const;

    Cell const* GetCell() const;
    int X() const { return m_X; }
    int Y() const { return m_Y; }

    GridRef North() const;
    GridRef South() const;
    GridRef East() const;
    GridRef West() const;

private:
    Grid const* m_Grid;
    int m_X;
    int m_Y;
};