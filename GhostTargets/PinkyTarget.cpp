#include "stdafx.h"
#include "PinkyTarget.h"

#include "Core/Vector.h"
#include "Maze/Direction.h"
#include "Maze/Movement.h"

PinkyTarget::PinkyTarget(Movement const& target) :
    m_Target(target)
{
}

sf::Vector2i PinkyTarget::It()
{
    int const k_TargetDistance = 4;

    // Move target ahead
    sf::Vector2i target = Add(m_Target.GetPosition(), k_TargetDistance, m_Target.GetDirection());

    // Overflow bug from original pacman, if target is moving north, also aim west.
    if (m_Target.GetDirection() == Direction::North)
    {
        target = Add(target, k_TargetDistance, Direction::West);
    }

    return target;
}
