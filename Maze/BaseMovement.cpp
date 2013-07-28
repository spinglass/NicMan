#include "stdafx.h"
#include "BaseMovement.h"

#include "Maze.h"
#include "Movement.h"

float const BaseMovement::k_Speed = 3.0f;

BaseMovement::BaseMovement(Base const& base, Movement& movement) :
    m_Base(base),
    m_Movement(movement),
    m_HomePosition(HomePosition::Middle),
    m_State(State::HomeUp)
{
}

BaseMovement::~BaseMovement()
{
}

void BaseMovement::ResetToHome()
{
    float x = 0.0f;
    switch(m_HomePosition)
    {
    case HomePosition::Left:
        x = m_Base.Left;
        break;
    case HomePosition::Middle:
        x = m_Base.ExitX;
        break;
    case HomePosition::Right:
        x = m_Base.Right;
        break;
    }

    m_Movement.Reset(x, m_Base.Middle);
    m_Movement.SetDirection(Direction::North);
    m_State = State::HomeUp;
}

void BaseMovement::Exit()
{
    m_State = State::Exit;
}

void BaseMovement::Update(float dt)
{
    float const delta = k_Speed * dt;
    sf::Vector2f pos = m_Movement.GetAbsolutePosition();
    Direction dir = m_Movement.GetDirection();

    switch(m_State)
    {
    case State::HomeUp:
        pos.y += delta;
        dir = Direction::North;
        if (pos.y >= m_Base.Top)
        {
            pos.y = m_Base.Top;
            m_State = State::HomeDown;
        }
        break;
    case State::HomeDown:
        pos.y -= delta;
        dir = Direction::South;
        if (pos.y <= m_Base.Bottom)
        {
            pos.y = m_Base.Bottom;
            m_State = State::HomeUp;
        }
        break;
    case State::Exit:
        if (pos.x < m_Base.ExitX)
        {
            pos.x += delta;
            if (pos.x >= m_Base.ExitX)
            {
                pos.x = m_Base.ExitX;
            }
        }
        else if (pos.x > m_Base.ExitX)
        {
            pos.x -= delta;
            if (pos.x <= m_Base.ExitX)
            {
                pos.x = m_Base.ExitX;
            }
        }
        else
        {
            pos.y += delta;
            if (pos.y >= m_Base.ExitY)
            {
                pos.y = m_Base.ExitY;
                m_State = State::Out;
            }
        }
    case State::Out:
        break;
    }

    m_Movement.Reset(pos.x, pos.y);
    m_Movement.SetDirection(dir);
}
