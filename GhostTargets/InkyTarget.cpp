#include "stdafx.h"
#include "InkyTarget.h"

#include "Maze/Direction.h"
#include "Maze/Movement.h"

InkyTarget::InkyTarget(Movement const& target, Movement const& partner) :
    m_Target(target),
    m_Partner(partner)
{
}

GridRef InkyTarget::It()
{
    int const k_TargetDistance = 2;

    // Move target ahead
    GridRef target = m_Target.GetPosition();
    for (int i = 0; i < k_TargetDistance; ++i)
    {
        target.MoveWithoutWrap(m_Target.GetDirection());
    }

    // Overflow bug from original pacman, if target is moving north, also aim west.
    if (m_Target.GetDirection() == Direction::North)
    {
        for (int i = 0; i < k_TargetDistance; ++i)
        {
            target.MoveWithoutWrap(Direction::West);
        }
    }

    // Now grab the difference between the other position (Blinky) and the adjusted target
    int const dx = target.X() - m_Partner.GetPosition().X();
    int const dy = target.Y() - m_Partner.GetPosition().Y();

    // Finally, move the target this distance in the other direction
    target.MoveWithoutWrap(dx, dy);

    return target;
}
