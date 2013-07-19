#include "stdafx.h"
#include "PinkyTarget.h"

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
    return target;
}
