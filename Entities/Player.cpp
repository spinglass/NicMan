#include "stdafx.h"
#include "Player.h"

#include "Maze/Cell.h"
#include "Maze/Direction.h"

Player::Player() :
    m_Direction(Direction::None),
    m_NextDirection(Direction::None),
    m_StopTimer(0.0f)
{
}

Player::~Player()
{
}

void Player::Load()
{
    m_Sprite.Load("Resources/player");
    m_Sprite.SetOriginToCentre();
    m_Sprite.Play();
    m_Sprite.SetShuttle(true);
    m_Sprite.SetSpeed(30.0f);

    //m_PillSound.Load("Resources/player_pill.wav");
    //m_PillSound.SetVolume(30.0f);
    //m_PowerPillSound.Load("Resources/player_powerpill.wav");
    //m_PowerPillSound.SetVolume(30.0f);
}

void Player::SetPosition(GridRef const& ref, float offsetX, float offsetY)
{
    assert(ref.CanPlayerPass());

    m_GridRef = ref;
    m_Offset.x = offsetX;
    m_Offset.y = offsetY;
    m_PreviousOffset = m_Offset;
}


void Player::Update(float dt)
{
    if (m_StopTimer > 0.0f)
    {
        m_StopTimer -= dt;
    }
    else
    {
        UpdateMovement(dt);
        UpdateNomming();
    }
    UpdateSprite(dt);
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

    bool const canGoNorth = northRef.CanPlayerPass();
    bool const canGoSouth = southRef.CanPlayerPass();
    bool const canGoEast = eastRef.CanPlayerPass();
    bool const canGoWest = westRef.CanPlayerPass();

    bool const upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool const downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool const leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool const rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    if (m_Direction == Direction::None)
    {
        // Not currently moving, check for starting
        if (upPressed && canGoNorth)
        {
            m_Direction = Direction::North;
        }
        else if (downPressed && canGoSouth)
        {
            m_Direction = Direction::South;
        }
        else if (rightPressed && canGoEast)
        {
            m_Direction = Direction::East;
        }
        else if (leftPressed && canGoWest)
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
                if (downPressed && !upPressed && canGoSouth)
                {
                    // Reverse
                    m_Direction = Direction::South;
                }
                else if (m_Offset.y < 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && canGoEast)
                    {
                        m_NextDirection = Direction::East;
                    }
                    else if (leftPressed && canGoWest)
                    {
                        m_NextDirection = Direction::West;
                    }
                }
                break;
            }
            case Direction::South:
            {
                if (upPressed && !downPressed && canGoNorth)
                {
                    // Reverse
                    m_Direction = Direction::North;
                }
                else if (m_Offset.y > 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && canGoEast)
                    {
                        m_NextDirection = Direction::East;
                    }
                    else if (leftPressed && canGoWest)
                    {
                        m_NextDirection = Direction::West;
                    }
                }
                break;
            }
            case Direction::East:
            {
                if (leftPressed && !rightPressed && canGoWest)
                {
                    // Reverse
                    m_Direction = Direction::West;
                }
                else if (m_Offset.x < 0.5f)
                {
                    // In first half of cell
                    if (upPressed && canGoNorth)
                    {
                        m_NextDirection = Direction::North;
                    }
                    else if (downPressed && canGoSouth)
                    {
                        m_NextDirection = Direction::South;
                    }
                }
                break;
            }
            case Direction::West:
            {
                if (rightPressed && !leftPressed && canGoEast)
                {
                    // Reverse
                    m_Direction = Direction::East;
                }
                else if (m_Offset.x > 0.5f)
                {
                    // In first half of cell
                    if (upPressed && canGoNorth)
                    {
                        m_NextDirection = Direction::North;
                    }
                    else if (downPressed && canGoSouth)
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
                if (canGoNorth)
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
                if (canGoSouth)
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
                if (canGoEast)
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
                if (canGoWest)
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
        bool transitionComplete = false;
        switch (m_Direction)
        {
            case Direction::North:
            {
                if (m_Offset.y >= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    transitionComplete = true;
                }
                break;
            }
            case Direction::South:
            {
                if (m_Offset.y <= 0.5f)
                {
                    m_Offset.y = 0.5f;
                    transitionComplete = true;
                }
                break;
            }
            case Direction::East:
            {
                if (m_Offset.x >= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    transitionComplete = true;
                }
                break;
            }
            case Direction::West:
            {
                if (m_Offset.x <= 0.5f)
                {
                    m_Offset.x = 0.5f;
                    transitionComplete = true;
                }
                break;
            }
        }
        if (transitionComplete)
        {
            m_Direction = m_NextDirection;
            m_NextDirection = Direction::None;
        }
    }
}

void Player::UpdateNomming()
{
    float const k_NomOffset = 0.25f;
    float const k_PillStopTime = 1.0f / 60.0f;
    float const k_PowerPillStopTime = 3.0f / 60.0f;

    if (m_GridRef.GetCell()->GetPill() || m_GridRef.GetCell()->GetPowerPill())
    {
        bool nom = false;
        switch(m_Direction)
        {
            case Direction::None:
                break;
            case Direction::North:
                nom = m_PreviousOffset.y < k_NomOffset && m_Offset.y >= k_NomOffset;
                break;
            case Direction::South:
                nom = m_PreviousOffset.y > 1.0f - k_NomOffset && m_Offset.y <= 1.0f - k_NomOffset;
                break;
            case Direction::East:
                nom = m_PreviousOffset.x < k_NomOffset && m_Offset.x >= k_NomOffset;
                break;
            case Direction::West:
                nom = m_PreviousOffset.x > 1.0f - k_NomOffset && m_Offset.x <= 1.0f - k_NomOffset;
                break;
        }

        if (nom)
        {
            if (m_GridRef.GetCell()->GetPill())
            {
                m_StopTimer = k_PillStopTime;
                m_PillSound.Play();
            }
            else
            {
                m_StopTimer = k_PowerPillStopTime;
                m_PowerPillSound.Play();
            }

            m_GridRef.GetCell()->Nom();
        }
    }

    m_PreviousOffset = m_Offset;
}

void Player::UpdateSprite(float dt)
{
    if (m_Direction != Direction::None)
    {
        float rot = 0.0f;
        switch(m_Direction)
        {
            case Direction::North:
                rot = 270.0f;
                break;
            case Direction::South:
                rot = 90.0f;
                break;
            case Direction::East:
                rot = 0.0f;
                break;
            case Direction::West:
                rot = 180.0f;
                break;
        }
        m_Sprite.SetRotation(rot);
        m_Sprite.Update(dt);
    }
}

void Player::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f gridPos = sf::Vector2f((float)m_GridRef.X(), (float)m_GridRef.Y()) + m_Offset;
    sf::Vector2f pos = transform.transformPoint(gridPos);
    m_Sprite.SetPosition(pos);
    m_Sprite.Draw(target);
}
