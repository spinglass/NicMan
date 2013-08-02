#include "stdafx.h"
#include "InkyTarget.h"

#include "Core/Vector.h"
#include "Maze/Direction.h"
#include "Maze/Movement.h"

InkyTarget::InkyTarget(Movement const& target, Movement const& partner) :
    m_Target(target),
    m_Partner(partner)
{
}

sf::Vector2i InkyTarget::It()
{
    int const k_TargetDistance = 2;

    // Move target ahead
    sf::Vector2i target = Add(m_Target.GetPosition(), k_TargetDistance, m_Target.GetDirection());

    // Overflow bug from original pacman, if target is moving north, also aim west.
    if (m_Target.GetDirection() == Direction::North)
    {
        target = Add(target, k_TargetDistance, Direction::West);
    }

    // Now grab the difference between the other position (Blinky) and the adjusted target
    sf::Vector2i const delta = target - m_Partner.GetPosition();

    // Finally, move the target this distance in the other direction
    target += delta;

    return target;
}
