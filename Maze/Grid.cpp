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

Cell* Grid::GetCell(int x, int y) const
{
    Cell* cell = nullptr;
    if (0 <= x && x < m_Width && 0 <= y && y < m_Height)
    {
        cell = m_Cells[x][y];
    }
    return cell;
}
