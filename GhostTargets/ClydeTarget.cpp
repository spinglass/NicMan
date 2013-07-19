#include "stdafx.h"
#include "ClydeTarget.h"

ClydeTarget::ClydeTarget(GridRef const& postion) :
    m_Position(postion)
{
}

GridRef ClydeTarget::It()
{
    return m_Position;
}
