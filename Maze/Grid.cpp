#include "stdafx.h"
#include "Grid.h"

#include "Cell.h"

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Initialise(int width, int height)
{
    m_Cells.resize(width);
    for (int x = 0; x < width; ++x)
    {
        m_Cells[x].resize(height);
    }

    m_Width = width;
    m_Height = height;
}

void Grid::AddCell(int x, int y, Cell& cell)
{
    m_Cells[x][y] = &cell;
}

void Grid::BuildWarpTargets()
{
    for (int i = 0; i < m_Width * m_Height; ++i)
    {
        int const x1 = i % m_Width;
        int const y1 = i / m_Width;
        Cell const* c1 = m_Cells[x1][y1];
        if (c1 && c1->GetWarpId() > 0)
        {
            // Cell has a warp id. Find its twin
            for (int j = i + 1; j < m_Width * m_Height; ++j)
            {
                int const x2 = j % m_Width;
                int const y2 = j / m_Width;
                Cell const* c2 = m_Cells[x2][y2];
                if (c2 && c1->GetWarpId() == c2->GetWarpId())
                {
                    // Point cells at one another
                    m_WarpTarget[c1] = GridRef(this, x2, y2);
                    m_WarpTarget[c2] = GridRef(this, x1, y1);
                }
            }
        }
    }
}

Cell* Grid::GetCell(int x, int y) const
{
    Cell* cell = nullptr;
    if (0 <= x && x < m_Width && 0 <= y && y < m_Height)
    {
        cell = m_Cells[x][y];
    }
    return cell;
}

GridRef Grid::GetWarpTarget(Cell const* cell) const
{
    GridRef target;
    auto it = m_WarpTarget.find(cell);
    if (it != m_WarpTarget.end())
    {
        target = it->second;
    }
    return target;
}
