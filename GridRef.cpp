#include "stdafx.h"
#include "GridRef.h"

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

Cell const* GridRef::Cell() const
{
    return m_Grid ? m_Grid->GetCell(m_X, m_Y) : nullptr;
}

GridRef::operator bool() const
{
    return (Cell() != nullptr);
}

GridRef GridRef::North() const
{
    return GridRef(m_Grid, m_X, m_Y + 1);
}

GridRef GridRef::South() const
{
    return GridRef(m_Grid, m_X, m_Y - 1);
}

GridRef GridRef::East() const
{
    return GridRef(m_Grid, m_X + 1, m_Y);
}

GridRef GridRef::West() const
{
    return GridRef(m_Grid, m_X - 1, m_Y);
}
