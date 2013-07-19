#include "stdafx.h"
#include "ClydeTarget.h"

ClydeTarget::ClydeTarget(GridRef const& postion, GridRef const& ownPostion, GridRef const& fleePosition) :
    m_Position(postion),
    m_OwnPosition(ownPostion),
    m_FleePosition(fleePosition)
{
}

GridRef ClydeTarget::It()
{
    float const k_FleeDist = 8.0f;

    // Get the difference between target and self
    float const distSq = DistanceSq(m_Position, m_OwnPosition);

    // Flee if too close
    bool const flee = (distSq < k_FleeDist * k_FleeDist);

    // Pick the right target
    GridRef const target = flee ? m_FleePosition : m_Position;
    return target;
}
