#include "stdafx.h"
#include "Cell.h"

Cell::Cell() :
    m_Type(Space),
    m_Pill(false),
    m_WasPill(false),
    m_PowerPill(false),
    m_WasPowerPill(false)
{
}

Cell Cell::Parse(char c)
{
    Cell cell;
    switch(c)
    {
    default:
    case ' ':
        cell.m_Type = Space;
        break;
    case '.':
        cell.m_Type = Space;
        cell.m_Pill = true;
        cell.m_WasPill = true;
        break;
    case '+':
        cell.m_Type = Space;
        cell.m_PowerPill = true;
        cell.m_WasPowerPill = true;
        break;
    case '#':
        cell.m_Type = Wall;
        break;
    case 'x':
        cell.m_Type = GhostBase;
        break;
    }

    return cell;
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
