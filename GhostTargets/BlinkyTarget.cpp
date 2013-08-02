#include "stdafx.h"
#include "BlinkyTarget.h"

#include "Maze/Movement.h"

BlinkyTarget::BlinkyTarget(Movement const& target) :
    m_Target(target)
{
}

sf::Vector2i BlinkyTarget::It()
{
    return m_Target.GetPosition();
}
