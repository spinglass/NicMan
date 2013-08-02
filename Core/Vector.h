#pragma once

#include "Maze/Direction.h"

template <typename T>
float DistanceSq(sf::Vector2<T> const& a, sf::Vector2<T> const& b)
{
    float const dx = (float)(a.x - b.x);
    float const dy = (float)(a.y - b.y);
    return (dx * dx) + (dy * dy);
}

template <typename T>
sf::Vector2<T> Add(sf::Vector2<T> v, T delta, Direction dir)
{
    switch(dir)
    {
    case Direction::North:
        v.y += delta;
        break;
    case Direction::East:
        v.x += delta;
        break;
    case Direction::South:
        v.y -= delta;
        break;
    case Direction::West:
        v.x -= delta;
        break;
    }

    return v;
}
