#include "stdafx.h"
#include "BlinkyTarget.h"

BlinkyTarget::BlinkyTarget(GridRef const& player) :
    m_Player(player)
{
}

GridRef BlinkyTarget::It()
{
    return m_Player;
}
