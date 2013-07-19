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
    GridRef target = m_Position;
    int const k_TargetDistance = 4;
    for (int i = 0; i < k_TargetDistance; ++i)
    {
        target.MoveWithoutWrap(m_Direction);
    }

    // Overflow bug from original pacman, if player is moving north, also aim west.
    if (m_Direction == Direction::North)
    {
        for (int i = 0; i < k_TargetDistance; ++i)
        {
            target.MoveWithoutWrap(Direction::West);
        }
    }

    return target;
}
