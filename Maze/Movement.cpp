#include "stdafx.h"
#include "Movement.h"

#include "Direction.h"
#include "Maze.h"

Movement::Movement(Maze const& maze, bool canTransition) :
    m_Maze(maze),
    m_CanTransition(canTransition),
    m_Direction(Direction::None),
    m_ExitDirection(Direction::None),
    m_Speed(1.0f),
    m_Transition(false),
    m_NextCellFunc(&Movement::NextCellDefault)
{
}

Movement::~Movement()
{
}

void Movement::Reset(float x, float y)
{
    int const gridX = (int)x;
    int const gridY = (int)y;
    m_Position = m_Maze.GetGridRef(gridX, gridY);
    m_Offset.x = x - (float)gridX;
    m_Offset.y = y - (float)gridY;
    m_Direction = Direction::None;
    m_ExitDirection = Direction::None;
    m_Transition = false;
}

void Movement::Update(float dt)
{
    // Move in current direction
    Move(m_Direction, dt);

    if (m_Transition)
    {
        assert(m_ExitDirection != Direction::None);

        // Transitioning to new direction, move that way too
        Move(m_ExitDirection, dt);

        // Check for completing transition
        switch (m_Direction)
        {
            case Direction::North:
            {
                if (m_Offset.y >= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    m_Transition = false;
                }
                break;
            }
            case Direction::South:
            {
                if (m_Offset.y <= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    m_Transition = false;
                }
                break;
            }
            case Direction::East:
            {
                if (m_Offset.x >= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    m_Transition = false;
                }
                break;
            }
            case Direction::West:
            {
                if (m_Offset.x <= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    m_Transition = false;
                }
                break;
            }
        }

        if (!m_Transition)
        {
            m_Direction = m_ExitDirection;
            m_ExitDirection = Direction::None;
        }
    }
    else
    {
        assert(!m_CanTransition || m_ExitDirection == Direction::None);

        // Normal movement
        GridRef const nextRef = m_Position.GetNext(m_Direction);
        bool const canContinue = nextRef.CanPlayerPass() && (m_ExitDirection == Direction::None || m_ExitDirection == m_Direction);

        // Check for next cell or a wall
        bool nextCell = false;
        switch (m_Direction)
        {
            case Direction::None:
                break;
            case Direction::North:
            {
                if (canContinue)
                {
                    if (m_Offset.y >= 1.0f)
                    {
                        // Next cell
                        m_Offset.y -= 1.0f;
                        nextCell = true;
                    }
                }
                else
                {
                    if (m_Offset.y >= 0.5f)
                    {
                        // Switch to exit direction, or stop, if none set
                        m_Offset.y = 0.5f;
                        m_Direction = m_ExitDirection;
                    }
                }
                break;
            }
            case Direction::South:
            {
                if (canContinue)
                {
                    if (m_Offset.y < 0.0f)
                    {
                        // Next cell
                        m_Offset.y += 1.0f;
                        nextCell = true;
                    }
                }
                else
                {
                    if (m_Offset.y <= 0.5f)
                    {
                        // Switch to exit direction, or stop, if none set
                        m_Offset.y = 0.5f;
                        m_Direction = m_ExitDirection;
                    }
                }
                break;
            }
            case Direction::East:
            {
                if (canContinue)
                {
                    if (m_Offset.x >= 1.0f)
                    {
                        // Next cell
                        m_Offset.x -= 1.0f;
                        nextCell = true;
                    }
                }
                else
                {
                    if (m_Offset.x >= 0.5f)
                    {
                        // Switch to exit direction, or stop, if none set
                        m_Offset.x = 0.5f;
                        m_Direction = m_ExitDirection;
                    }
                }
                break;
            }
            case Direction::West:
            {
                if (canContinue)
                {
                    if (m_Offset.x < 0.0f)
                    {
                        // Next cell
                        m_Offset.x += 1.0f;
                        nextCell = true;
                    }
                }
                else
                {
                    if (m_Offset.x <= 0.5f)
                    {
                        // Switch to exit direction, or stop, if none set
                        m_Offset.x = 0.5f;
                        m_Direction = m_ExitDirection;
                    }
                }
                break;
            }
        }

        if (nextCell)
        {
            m_Position = nextRef;

            // Determine direction to exit next cell.
            // Maybe none if no decisions has yet been taken.
            m_ExitDirection = m_NextCellFunc();
        }
    }
}

sf::Vector2f Movement::GetAbsolutePosition() const
{
    sf::Vector2f const cellOrigin((float)m_Position.X(), (float)m_Position.Y());
    return (cellOrigin + m_Offset);
}

void Movement::SetDirection(Direction direction)
{
    m_Direction = direction;
}

void Movement::SetExitDirection(Direction direction)
{
    m_ExitDirection = direction;
    m_Transition = m_CanTransition;
}

void Movement::Move(Direction dir, float dt)
{
    float const delta = m_Speed * dt;

    switch(dir)
    {
        case Direction::None:
            break;
        case Direction::North:
        {
            m_Offset.y += delta;
            break;
        }
        case Direction::South:
        {
            m_Offset.y -= delta;
            break;
        }
        case Direction::East:
        {
            m_Offset.x += delta;
            break;
        }
        case Direction::West:
        {
            m_Offset.x -= delta;
            break;
        }
    }
}

Direction Movement::NextCellDefault()
{
    return Direction::None;
}
