#pragma once

#include <functional>

#include "Direction.h"
#include "GridRef.h"

class Movement
{
public:
    typedef std::function<Direction()> NextCellFunc;

    Movement(bool canTransition);
    virtual ~Movement();

    void Reset(GridRef const& position, float x, float y);
    void Update(float dt);

    GridRef const& GetPosition() const { return m_Position; }
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

    bool const m_CanTransition;
    GridRef m_Position;
    sf::Vector2f m_Offset;
    Direction m_Direction;
    Direction m_ExitDirection;
    float m_Speed;
    bool m_Transition;
    NextCellFunc m_NextCellFunc;
};
