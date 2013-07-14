#include "stdafx.h"
#include "GridRef.h"

#include "Cell.h"
#include "Grid.h"

GridRef::GridRef() :
    m_Grid(nullptr),
    m_X(0),
    m_Y(0)
{
}

GridRef::GridRef(Grid const* grid, int x, int y) :
    m_Grid(grid),
    m_X(x),
    m_Y(y)
{
}

GridRef::~GridRef()
{
}

Cell* GridRef::GetCell() const
{
    return m_Grid ? m_Grid->GetCell(m_X, m_Y) : nullptr;
}

GridRef::operator bool() const
{
    return (GetCell() != nullptr);
}

bool GridRef::CanPlayerPass() const
{
    Cell const* cell = GetCell();
    return cell ? !cell->IsGhostBase() : false;
}


GridRef GridRef::North() const
{
    int y = (m_Y + 1);
    if (m_Grid)
    {
        y = y % m_Grid->GetHeight();
    }
    return GridRef(m_Grid, m_X, y);
}

GridRef GridRef::South() const
{
    int y = (m_Y - 1);
    if (m_Grid)
    {
        y = (y + m_Grid->GetHeight()) % m_Grid->GetHeight();
    }
    return GridRef(m_Grid, m_X, y);
}

GridRef GridRef::East() const
{
    int x = (m_X + 1);
    if (m_Grid)
    {
        x = x % m_Grid->GetWidth();
    }
    return GridRef(m_Grid, x, m_Y);
}

GridRef GridRef::West() const
{
    int x = (m_X - 1);
    if (m_Grid)
    {
        x = (x + m_Grid->GetWidth()) % m_Grid->GetWidth();
    }
    return GridRef(m_Grid, x, m_Y);
}
