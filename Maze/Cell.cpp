#include "stdafx.h"
#include "Cell.h"

Cell::Cell(char c) :
    m_GhostBase(false),
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
    case '+':
        // Empty cell
        break;
    case '=':
        m_Tunnel = true;
        break;
    case 'o':
        m_Pill = true;
        m_WasPill = true;
        break;
    case '#':
        m_PowerPill = true;
        m_WasPowerPill = true;
        break;
    case 'x':
        m_GhostBase = true;
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
