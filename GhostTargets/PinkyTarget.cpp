#include "stdafx.h"
#include "PinkyTarget.h"

#include "Maze/Direction.h"

PinkyTarget::PinkyTarget(GridRef const& postion, Direction const& direction) :
    m_Position(postion),
    m_Direction(direction)
{
}

GridRef PinkyTarget::It()
{
    int const k_TargetDistance = 4;

    // Move target ahead
    GridRef target = m_Position;
    for (int i = 0; i < k_TargetDistance; ++i)
    {
        target.MoveWithoutWrap(m_Direction);
    }

    // Overflow bug from original pacman, if target is moving north, also aim west.
    if (m_Direction == Direction::North)
    {
        for (int i = 0; i < k_TargetDistance; ++i)
        {
            target.MoveWithoutWrap(Direction::West);
        }
    }

    return target;
}
