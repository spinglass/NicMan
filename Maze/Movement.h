#pragma once

#include <functional>

#include "Direction.h"

class Cell;
class Maze;

class Movement
{
public:
    typedef std::function<Direction()> NextCellFunc;

    Movement(Maze const& maze, bool canTransition);
    virtual ~Movement();

    void Reset(float x, float y);
    void Update(float dt);

    Cell* GetCell() { return m_Cell; }
    Cell const* GetCell() const { return m_Cell; }
    sf::Vector2i GetPosition() const;
    sf::Vector2f GetOffset() const { return m_Offset; }
    Direction GetDirection() const { return m_Direction; }
    Direction GetExitDirection() const { return m_ExitDirection; }
    sf::Vector2f GetAbsolutePosition() const;

    void SetSpeed(float speed) { m_Speed = speed; }
    void SetDirection(Direction direction);
    void SetExitDirection(Direction direction);
    void SetNextCellFunc(NextCellFunc& func) { m_NextCellFunc = func; }

private:
    Movement(Movement&);
    Movement& operator=(Movement&);

    void Move(Direction dir, float dt);
    static Direction NextCellDefault();

    Maze const& m_Maze;
    Cell* m_Cell;
    bool const m_CanTransition;
    sf::Vector2f m_Offset;
    Direction m_Direction;
    Direction m_ExitDirection;
    float m_Speed;
    bool m_Transition;
    NextCellFunc m_NextCellFunc;
};
