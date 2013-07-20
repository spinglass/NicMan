#include "stdafx.h"
#include "ClydeTarget.h"

#include "Maze/Movement.h"

ClydeTarget::ClydeTarget(Movement const& target, Movement const& self, GridRef const& fleePosition) :
    m_Target(target),
    m_Self(self),
    m_FleePosition(fleePosition)
{
}

GridRef ClydeTarget::It()
{
    float const k_FleeDist = 8.0f;

    // Get the difference between target and self
    float const distSq = DistanceSq(m_Target.GetPosition(), m_Self.GetPosition());

    // Flee if too close
    bool const flee = (distSq < k_FleeDist * k_FleeDist);

    // Pick the right target
    GridRef const target = flee ? m_FleePosition : m_Target.GetPosition();
    return target;
}
