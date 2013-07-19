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

class DirectionIter
{
public:
    Direction operator*() const { return (Direction)m_Value; }
    void operator++() { ++m_Value; }
    bool operator!=(DirectionIter rhs) { return m_Value != rhs.m_Value; }

    static DirectionIter Begin();
    static DirectionIter End();

private:
    DirectionIter(int value) : m_Value(value) {}

    int m_Value;
};
