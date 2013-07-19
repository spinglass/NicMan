#include "stdafx.h"
#include "Direction.h"

Direction Opposite(Direction dir)
{
    switch(dir)
    {
        case Direction::North:
            return Direction::South;
        case Direction::South:
            return Direction::North;
        case Direction::East:
            return Direction::West;
        case Direction::West:
            return Direction::East;
    }
    return Direction::None;
}
