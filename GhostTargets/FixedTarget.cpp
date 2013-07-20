#include "stdafx.h"
#include "FixedTarget.h"

FixedTarget::FixedTarget(GridRef const& target) :
    m_Target(target)
{
}

GridRef FixedTarget::It()
{
    return m_Target;
}
