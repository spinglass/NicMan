#include "stdafx.h"
#include "InkyTarget.h"

InkyTarget::InkyTarget(GridRef const& postion) :
    m_Position(postion)
{
}

GridRef InkyTarget::It()
{
    return m_Position;
}
