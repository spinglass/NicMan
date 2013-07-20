#include "stdafx.h"
#include "PinkyTarget.h"

#include "Maze/Direction.h"
#include "Maze/Movement.h"

PinkyTarget::PinkyTarget(Movement const& target) :
    m_Target(target)
{
}

GridRef PinkyTarget::It()
{
    int const k_TargetDistance = 4;

    // Move target ahead
    GridRef postion = m_Target.GetPosition();
    for (int i = 0; i < k_TargetDistance; ++i)
    {
        postion.MoveWithoutWrap(m_Target.GetDirection());
    }

    // Overflow bug from original pacman, if target is moving north, also aim west.
    if (m_Target.GetDirection() == Direction::North)
    {
        for (int i = 0; i < k_TargetDistance; ++i)
        {
            postion.MoveWithoutWrap(Direction::West);
        }
    }

    return postion;
}
