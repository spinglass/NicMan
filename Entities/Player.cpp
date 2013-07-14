#include "stdafx.h"
#include "Player.h"

Player::Player() :
    m_Direction(Direction::None),
    m_NextDirection(Direction::None)
{
}

Player::~Player()
{
}

void Player::Load()
{
    m_Sprite.Load("Resources/player_00.png");
    m_Sprite.SetOriginToCentre();
}

void Player::SetPosition(GridRef const& ref, float offsetX, float offsetY)
{
    assert(ref.CanPlayerPass());

    m_GridRef = ref;
    m_Offset.x = offsetX;
    m_Offset.y = offsetY;
}


void Player::Update(float dt)
{
    UpdateMovement(dt);
}

void Player::Move(Direction dir, float dt)
{
    float const k_Speed = 8.0f;
    float const delta = k_Speed * dt;

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

void Player::UpdateMovement(float dt)
{
    GridRef const northRef = m_GridRef.North();
    GridRef const southRef = m_GridRef.South();
    GridRef const eastRef = m_GridRef.East();
    GridRef const westRef = m_GridRef.West();

    bool const north = northRef.CanPlayerPass();
    bool const south = southRef.CanPlayerPass();
    bool const east = eastRef.CanPlayerPass();
    bool const west = westRef.CanPlayerPass();

    bool const upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool const downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool const leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool const rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    if (m_Direction== Direction::None)
    {
        // Not currently moving, check for starting
        if (upPressed && north)
        {
            m_Direction = Direction::North;
        }
        else if (downPressed && south)
        {
            m_Direction = Direction::South;
        }
        else if (rightPressed && east)
        {
            m_Direction = Direction::East;
        }
        else if (leftPressed && west)
        {
            m_Direction = Direction::West;
        }
    }
    else if (m_NextDirection == Direction::None)
    {
        // Check for direction change
        switch (m_Direction)
        {
            case Direction::North:
            {
                if (downPressed && !upPressed && south)
                {
                    // Reverse
                    m_Direction = Direction::South;
                }
                else if (m_Offset.y < 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && east)
                    {
                        m_NextDirection = Direction::East;
                    }
                    else if (leftPressed && west)
                    {
                        m_NextDirection = Direction::West;
                    }
                }
                break;
            }
            case Direction::South:
            {
                if (upPressed && !downPressed && north)
                {
                    // Reverse
                    m_Direction = Direction::North;
                }
                else if (m_Offset.y > 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && east)
                    {
                        m_NextDirection = Direction::East;
                    }
                    else if (leftPressed && west)
                    {
                        m_NextDirection = Direction::West;
                    }
                }
                break;
            }
            case Direction::East:
            {
                if (leftPressed && !rightPressed && west)
                {
                    // Reverse
                    m_Direction = Direction::West;
                }
                else if (m_Offset.x < 0.5f)
                {
                    // In first half of cell
                    if (upPressed && north)
                    {
                        m_NextDirection = Direction::North;
                    }
                    else if (downPressed && south)
                    {
                        m_NextDirection = Direction::South;
                    }
                }
                break;
            }
            case Direction::West:
            {
                if (rightPressed && !leftPressed && east)
                {
                    // Reverse
                    m_Direction = Direction::East;
                }
                else if (m_Offset.x > 0.5f)
                {
                    // In first half of cell
                    if (upPressed && north)
                    {
                        m_NextDirection = Direction::North;
                    }
                    else if (downPressed && south)
                    {
                        m_NextDirection = Direction::South;
                    }
                }
                break;
            }
        }
    }

    // Move in current direction
    Move(m_Direction, dt);
    if (m_NextDirection == Direction::None)
    {
        // Check for next cell or a wall
        switch (m_Direction)
        {
            case Direction::North:
            {
                if (north)
                {
                    if (m_Offset.y >= 1.0f)
                    {
                        // Next cell
                        m_Offset.y -= 1.0f;
                        m_GridRef = northRef;
                    }
                }
                else
                {
                    if (m_Offset.y >= 0.5f)
                    {
                        // Stop
                        m_Offset.y = 0.5f;
                        m_Direction = Direction::None;
                    }
                }
                break;
            }
            case Direction::South:
            {
                if (south)
                {
                    if (m_Offset.y < 0.0f)
                    {
                        // Next cell
                        m_Offset.y += 1.0f;
                        m_GridRef = southRef;
                    }
                }
                else
                {
                    if (m_Offset.y <= 0.5f)
                    {
                        // Stop
                        m_Offset.y = 0.5f;
                        m_Direction = Direction::None;
                    }
                }
                break;
            }
            case Direction::East:
            {
                if (east)
                {
                    if (m_Offset.x >= 1.0f)
                    {
                        // Next cell
                        m_Offset.x -= 1.0f;
                        m_GridRef = eastRef;
                    }
                }
                else
                {
                    if (m_Offset.x >= 0.5f)
                    {
                        // Stop
                        m_Offset.x = 0.5f;
                        m_Direction = Direction::None;
                    }
                }
                break;
            }
            case Direction::West:
            {
                if (west)
                {
                    if (m_Offset.x < 0.0f)
                    {
                        // Next cell
                        m_Offset.x += 1.0f;
                        m_GridRef = westRef;
                    }
                }
                else
                {
                    if (m_Offset.x <= 0.5f)
                    {
                        // Stop
                        m_Offset.x = 0.5f;
                        m_Direction = Direction::None;
                    }
                }
                break;
            }
        }
    }
    else
    {
        // Transitioning to new direction, move that way too
        Move(m_NextDirection, dt);

        // Check for completing transition
        switch (m_Direction)
        {
            case Direction::North:
            {
                if (m_Offset.y >= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    m_Direction = m_NextDirection;
                    m_NextDirection = Direction::None;
                }
                break;
            }
            case Direction::South:
            {
                if (m_Offset.y <= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    m_Direction = m_NextDirection;
                    m_NextDirection = Direction::None;
                }
                break;
            }
            case Direction::East:
            {
                if (m_Offset.x >= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    m_Direction = m_NextDirection;
                    m_NextDirection = Direction::None;
                }
                break;
            }
            case Direction::West:
            {
                if (m_Offset.x <= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    m_Direction = m_NextDirection;
                    m_NextDirection = Direction::None;
                }
                break;
            }
        }
    }
}

void Player::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f gridPos = sf::Vector2f((float)m_GridRef.X(), (float)m_GridRef.Y()) + m_Offset;
    sf::Vector2f pos = transform.transformPoint(gridPos);
    m_Sprite.SetPosition(pos);
    m_Sprite.Draw(target);
}
