#include "stdafx.h"
#include "Cell.h"

#include "Direction.h"

Cell::Cell(char c, int x, int y) :
    m_X(x),
    m_Y(y),
    m_North(nullptr),
    m_East(nullptr),
    m_South(nullptr),
    m_West(nullptr),
    m_Open(false),
    m_Pill(false),
    m_WasPill(false),
    m_PowerPill(false),
    m_WasPowerPill(false),
    m_Tunnel(false)
{
    Parse(c);
}

bool Cell::IsCell(char c)
{
    return (c == '+' || c == '=' || c == 'o' || c == '#' || c == 'x');
}

void Cell::Parse(char c)
{
    switch(c)
    {
    default:
        break;
    case '+':
        // Empty cell
        m_Open = true;
        break;
    case '=':
        m_Open = true;
        m_Tunnel = true;
        break;
    case 'o':
        m_Open = true;
        m_Pill = true;
        m_WasPill = true;
        break;
    case '#':
        m_Open = true;
        m_PowerPill = true;
        m_WasPowerPill = true;
        break;
    }
}

Cell* Cell::GetNext(Direction dir) const
{
    Cell* cell = nullptr;
    switch(dir)
    {
    default:
        break;
    case Direction::North:
        cell = m_North;
        break;
    case Direction::East:
        cell = m_East;
        break;
    case Direction::South:
        cell = m_South;
        break;
    case Direction::West:
        cell = m_West;
        break;
    }
    return cell;
}

void Cell::SetNext(Direction dir, Cell* next)
{
    switch(dir)
    {
    case Direction::North:
        m_North = next;
        break;
    case Direction::East:
        m_East = next;
        break;
    case Direction::South:
        m_South = next;
        break;
    case Direction::West:
        m_West = next;
        break;
    }
}

void Cell::Nom()
{
    m_Pill = false;
    m_PowerPill = false;
}

void Cell::Reset()
{
    m_Pill = m_WasPill;
    m_PowerPill = m_WasPowerPill;
}
