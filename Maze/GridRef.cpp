#include "stdafx.h"
#include "GridRef.h"

#include "Cell.h"
#include "Direction.h"
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

bool GridRef::operator==(GridRef const& rhs) const
{
    return (m_Grid == rhs.m_Grid) && (m_X == rhs.m_X) && (m_Y == rhs.m_Y);
}

bool GridRef::CanPlayerPass() const
{
    Cell const* cell = GetCell();
    return cell ? !cell->IsGhostBase() : false;
}

void GridRef::Wrap()
{
    if (m_Grid)
    {
        m_X = (m_X + m_Grid->GetWidth()) % m_Grid->GetWidth();
        m_Y = (m_Y + m_Grid->GetHeight()) % m_Grid->GetHeight();
    }
}

void GridRef::MoveWithoutWrap(Direction dir)
{
    switch(dir)
    {
        case Direction::North:
            ++m_Y;
            break;
        case Direction::South:
            --m_Y;
            break;
        case Direction::East:
            ++m_X;
            break;
        case Direction::West:
            --m_X;
            break;
    }
}

void GridRef::Move(Direction dir)
{
    MoveWithoutWrap(dir);
    Wrap();
}

void GridRef::MoveWithoutWrap(int dx, int dy)
{
    m_X += dx;
    m_Y += dy;
}

void GridRef::Move(int dx, int dy)
{
    MoveWithoutWrap(dx, dy);
    Wrap();
}

GridRef GridRef::GetNextWithoutWrap(Direction dir) const
{
    GridRef gridRef(*this);
    gridRef.MoveWithoutWrap(dir);
    return gridRef;
}

GridRef GridRef::GetNext(Direction dir) const
{
    GridRef gridRef(*this);
    gridRef.Move(dir);
    return gridRef;
}

GridRef GridRef::North() const
{
    return GetNext(Direction::North);
}

GridRef GridRef::South() const
{
    return GetNext(Direction::South);
}

GridRef GridRef::East() const
{
    return GetNext(Direction::East);
}

GridRef GridRef::West() const
{ 
   return GetNext(Direction::West);
}

float DistanceSq(GridRef const& a, GridRef const& b)
{
    float const dx = (float)(a.X() - b.X());
    float const dy = (float)(a.Y() - b.Y());
    return (dx * dx) + (dy * dy);
}
