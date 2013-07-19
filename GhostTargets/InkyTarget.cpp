#include "stdafx.h"
#include "InkyTarget.h"

#include "Maze/Direction.h"

InkyTarget::InkyTarget(GridRef const& postion, Direction const& direction, GridRef const& otherPosition) :
    m_Position(postion),
    m_Direction(direction),
    m_OtherPosition(otherPosition)
{
}

GridRef InkyTarget::It()
{
    int const k_TargetDistance = 2;

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

    // Now grab the difference between the other position (Blinky) and the adjusted target
    int const dx = target.X() - m_OtherPosition.X();
    int const dy = target.Y() - m_OtherPosition.Y();

    // Finally, move the target this distance in the other direction
    target.MoveWithoutWrap(dx, dy);

    return target;
}
