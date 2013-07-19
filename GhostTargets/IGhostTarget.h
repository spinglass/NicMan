#pragma once

#include "Maze/GridRef.h"

class IGhostTarget
{
public:
    virtual ~IGhostTarget() {}
    virtual GridRef It() = 0;
};
