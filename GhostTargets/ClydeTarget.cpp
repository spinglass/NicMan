#include "stdafx.h"
#include "ClydeTarget.h"

#include "Core/Vector.h"
#include "Maze/Movement.h"

ClydeTarget::ClydeTarget(Movement const& target, Movement const& self, sf::Vector2i fleePosition) :
    m_Target(target),
    m_Self(self),
    m_FleePosition(fleePosition)
{
}

sf::Vector2i ClydeTarget::It()
{
    float const k_FleeDist = 8.0f;

    // Get the difference between target and self
    float const distSq = DistanceSq(m_Target.GetPosition(), m_Self.GetPosition());

    // Flee if too close
    bool const flee = (distSq < k_FleeDist * k_FleeDist);

    // Pick the right target
    sf::Vector2i const target = flee ? m_FleePosition : m_Target.GetPosition();
    return target;
}
