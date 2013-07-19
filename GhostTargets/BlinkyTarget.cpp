#include "stdafx.h"
#include "BlinkyTarget.h"

BlinkyTarget::BlinkyTarget(GridRef const& postion) :
    m_Position(postion)
{
}

GridRef BlinkyTarget::It()
{
    return m_Position;
}
