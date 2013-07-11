#include "stdafx.h"
#include "Grid.h"

Grid::Grid()
{
}

Grid::~Grid()
{
}

void Grid::Initialise(int cols, int rows)
{
    m_Cells.resize(cols);
    for (int col = 0; col < cols; ++col)
    {
        m_Cells[col].resize(rows);
    }
}

void Grid::AddCell(int col, int row, Cell const& cell)
{
    m_Cells[col][row] = &cell;
}

Cell const* Grid::GetCell(int col, int row) const
{
    return m_Cells[col][row];
}
