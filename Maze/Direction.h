#pragma once

#include <functional>

enum class Direction
{
    None,
    North,
    East,
    South,
    West,
};

Direction Opposite(Direction dir);
