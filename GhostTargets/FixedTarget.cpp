#include "stdafx.h"
#include "FixedTarget.h"

FixedTarget::FixedTarget(sf::Vector2i target) :
    m_Target(target)
{
}

sf::Vector2i FixedTarget::It()
{
    return m_Target;
}
