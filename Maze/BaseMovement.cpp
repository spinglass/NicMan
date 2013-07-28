#include "stdafx.h"
#include "BaseMovement.h"

#include "Maze.h"
#include "Movement.h"

float const BaseMovement::k_HomeSpeed = 3.0f;
float const BaseMovement::k_ExitSpeed = 5.0f;
float const BaseMovement::k_EnterSpeed = 4.0f;

BaseMovement::BaseMovement(Base const& base, Movement& movement) :
    m_Base(base),
    m_Movement(movement),
    m_HomePosition(HomePosition::Middle),
    m_State(State::Home),
    m_Capture(false)
{
}

BaseMovement::~BaseMovement()
{
}

bool BaseMovement::IsOut() const
{
    return m_State == State::Out;
}

bool BaseMovement::IsHome() const
{
    return m_State == State::Home;
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
    m_State = State::Home;
}

void BaseMovement::ResetToOut()
{
    m_State = State::Out;
}

void BaseMovement::Exit()
{
    if (m_State == State::Home)
    {
        m_State = State::Exit;
    }
}

void BaseMovement::Capture()
{
    m_Capture = true;
}

void BaseMovement::Update(float dt)
{
    switch(m_State)
    {
    case State::Home:
        UpdateHome(dt);
        break;
    case State::Exit:
        UpdateExit(dt);
    case State::Out:
        if (m_Capture)
        {
            UpdateCapture();
        }
        break;
    case State::Enter:
        UpdateEnter(dt);
        break;
    }

    m_PreviousPosition = m_Movement.GetAbsolutePosition();
}

void BaseMovement::UpdateHome(float dt)
{
    float const delta = k_HomeSpeed * dt;
    sf::Vector2f pos = m_Movement.GetAbsolutePosition();
    Direction dir = m_Movement.GetDirection();

    if (dir == Direction::North)
    {
        pos.y += delta;
        if (pos.y >= m_Base.Top)
        {
            // Switch to south
            pos.y = m_Base.Top;
            dir = Direction::South;
        }
    }
    else
    {
        pos.y -= delta;
        if (pos.y <= m_Base.Bottom)
        {
            // Switch to north
            pos.y = m_Base.Bottom;
            dir = Direction::North;
        }
    }

    m_Movement.Reset(pos.x, pos.y);
    m_Movement.SetDirection(dir);
}

void BaseMovement::UpdateExit(float dt)
{
    float const delta = k_ExitSpeed * dt;
    sf::Vector2f pos = m_Movement.GetAbsolutePosition();
    Direction dir;

    if (pos.x < m_Base.ExitX)
    {
        // Moving to centre from the left
        dir = Direction::East;
        pos.x += delta;
        if (pos.x >= m_Base.ExitX)
        {
            // Lock to centre
            pos.x = m_Base.ExitX;
        }
    }
    else if (pos.x > m_Base.ExitX)
    {
        // Moving to centre from the right
        dir = Direction::West;
        pos.x -= delta;
        if (pos.x <= m_Base.ExitX)
        {
            // Lock to centre
            pos.x = m_Base.ExitX;
        }
    }
    else
    {
        // Moving up along the centre line
        dir = Direction::North;
        pos.y += delta;
        if (pos.y >= m_Base.ExitY)
        {
            // Out of the base
            pos.y = m_Base.ExitY;
            m_State = State::Out;
        }
    }

    m_Movement.Reset(pos.x, pos.y);
    m_Movement.SetDirection(dir);
}

void BaseMovement::UpdateCapture()
{
    sf::Vector2f pos = m_Movement.GetAbsolutePosition();
    if (pos.y == m_Base.ExitY)
    {
        if ((pos.x >= m_Base.ExitX && m_PreviousPosition.x <= m_Base.ExitX) ||
            (pos.x <= m_Base.ExitX && m_PreviousPosition.x >= m_Base.ExitX))
        {
            // Got ya!
            m_Movement.Reset(m_Base.ExitX, m_Base.ExitY);
            m_Movement.SetDirection(Direction::South);
            m_State = State::Enter;
            m_Capture = false;
        }
    }
}

void BaseMovement::UpdateEnter(float dt)
{
    float const delta = k_EnterSpeed * dt;
    sf::Vector2f pos = m_Movement.GetAbsolutePosition();
    Direction dir = m_Movement.GetDirection();

    if (pos.y > m_Base.Middle)
    {
        // Moving down to middle from the exit
        dir = Direction::South;
        pos.y -= delta;
        if (pos.y <= m_Base.Middle)
        {
            // Lock to middle
            pos.y = m_Base.Middle;
        }
    }
    else
    {
        switch (m_HomePosition)
        {
        case HomePosition::Left:
            // Move left
            dir = Direction::West;
            pos.x -= delta;
            if (pos.x <= m_Base.Left)
            {
                pos.x = m_Base.Left;
                m_State = State::Home;
            }
            break;
        case HomePosition::Middle:
            // Home immediately
            m_State = State::Home;
            break;
        case HomePosition::Right:
            // Move right
            dir = Direction::East;
            pos.x += delta;
            if (pos.x >= m_Base.Right)
            {
                pos.x = m_Base.Right;
                m_State = State::Home;
            }
            break;
        }
    }

    m_Movement.Reset(pos.x, pos.y);
    m_Movement.SetDirection(dir);
}
