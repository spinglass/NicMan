#include "stdafx.h"
#include "Player.h"

#include "Maze/Cell.h"
#include "Maze/Direction.h"

Player::Player(Maze const& maze) :
    m_Movement(maze, true),
    m_StopTimer(0.0f),
    m_AtePill(false),
    m_AtePowerPill(false)
{
}

Player::~Player()
{
}

void Player::Load()
{
    m_Sprite.Load("Player/Player");
    m_Sprite.SetOriginToCentre();
    m_Sprite.SetShuttle(true);

    //m_PillSound.Load("Resources/player_pill.wav");
    //m_PillSound.SetVolume(30.0f);
    //m_PowerPillSound.Load("Resources/player_powerpill.wav");
    //m_PowerPillSound.SetVolume(30.0f);
}

void Player::Restart(float x, float y)
{
    m_Movement.Reset(x, y);
    assert(m_Movement.GetPosition().CanPlayerPass());

    m_PreviousOffset = m_Movement.GetOffset();
    m_StopTimer = 0.0f;
    m_Sprite.Reset();
    m_Sprite.Play();
}

void Player::SetSpeed(float speed)
{
    float const k_SpriteSpeedFactor = 6.0f;

    m_Movement.SetSpeed(speed);
    m_Sprite.SetSpeed(speed * k_SpriteSpeedFactor);

}

void Player::Update(float dt)
{
    m_AtePill = false;
    m_AtePowerPill = false;

    if (m_StopTimer > 0.0f)
    {
        m_StopTimer -= dt;
    }
    else
    {
        UpdateControls();
        m_Movement.Update(dt);
        UpdateNomming();
    }
    UpdateSprite(dt);
}

void Player::UpdateControls()
{
    GridRef const& position = m_Movement.GetPosition();
    bool const canGoNorth = position.North().CanPlayerPass();
    bool const canGoSouth = position.South().CanPlayerPass();
    bool const canGoEast = position.East().CanPlayerPass();
    bool const canGoWest = position.West().CanPlayerPass();

    bool const upPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool const downPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool const leftPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool const rightPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);

    if (m_Movement.GetDirection() == Direction::None)
    {
        // Not currently moving, check for starting
        if (upPressed && canGoNorth)
        {
            m_Movement.SetDirection(Direction::North);
        }
        else if (downPressed && canGoSouth)
        {
            m_Movement.SetDirection(Direction::South);
        }
        else if (rightPressed && canGoEast)
        {
            m_Movement.SetDirection(Direction::East);
        }
        else if (leftPressed && canGoWest)
        {
            m_Movement.SetDirection(Direction::West);
        }
    }
    else if (m_Movement.GetExitDirection() == Direction::None)
    {
        // Not transitioning to another direction, check for direction change

        sf::Vector2f offset = m_Movement.GetOffset();
        switch (m_Movement.GetDirection())
        {
            case Direction::North:
            {
                if (downPressed && !upPressed && canGoSouth)
                {
                    // Reverse
                    m_Movement.SetDirection(Direction::South);
                }
                else if (offset.y < 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && canGoEast)
                    {
                        m_Movement.SetExitDirection(Direction::East);
                    }
                    else if (leftPressed && canGoWest)
                    {
                        m_Movement.SetExitDirection(Direction::West);
                    }
                }
                break;
            }
            case Direction::South:
            {
                if (upPressed && !downPressed && canGoNorth)
                {
                    // Reverse
                    m_Movement.SetDirection(Direction::North);
                }
                else if (offset.y > 0.5f)
                {
                    // In first half of cell
                    if (rightPressed && canGoEast)
                    {
                        m_Movement.SetExitDirection(Direction::East);
                    }
                    else if (leftPressed && canGoWest)
                    {
                        m_Movement.SetExitDirection(Direction::West);
                    }
                }
                break;
            }
            case Direction::East:
            {
                if (leftPressed && !rightPressed && canGoWest)
                {
                    // Reverse
                    m_Movement.SetDirection(Direction::West);
                }
                else if (offset.x < 0.5f)
                {
                    // In first half of cell
                    if (upPressed && canGoNorth)
                    {
                        m_Movement.SetExitDirection(Direction::North);
                    }
                    else if (downPressed && canGoSouth)
                    {
                        m_Movement.SetExitDirection(Direction::South);
                    }
                }
                break;
            }
            case Direction::West:
            {
                if (rightPressed && !leftPressed && canGoEast)
                {
                    // Reverse
                    m_Movement.SetDirection(Direction::East);
                }
                else if (offset.x > 0.5f)
                {
                    // In first half of cell
                    if (upPressed && canGoNorth)
                    {
                        m_Movement.SetExitDirection(Direction::North);
                    }
                    else if (downPressed && canGoSouth)
                    {
                        m_Movement.SetExitDirection(Direction::South);
                    }
                }
                break;
            }
        }
    }
}

void Player::UpdateNomming()
{
    float const k_NomOffset = 0.25f;
    float const k_PillStopTime = 1.0f / 60.0f;
    float const k_PowerPillStopTime = 3.0f / 60.0f;

    GridRef const& position = m_Movement.GetPosition();
    sf::Vector2f const offset = m_Movement.GetOffset();

    if (position.GetCell()->GetPill() || position.GetCell()->GetPowerPill())
    {
        bool nom = false;
        switch(m_Movement.GetDirection())
        {
            case Direction::None:
                break;
            case Direction::North:
                nom = m_PreviousOffset.y < k_NomOffset && offset.y >= k_NomOffset;
                break;
            case Direction::South:
                nom = m_PreviousOffset.y > 1.0f - k_NomOffset && offset.y <= 1.0f - k_NomOffset;
                break;
            case Direction::East:
                nom = m_PreviousOffset.x < k_NomOffset && offset.x >= k_NomOffset;
                break;
            case Direction::West:
                nom = m_PreviousOffset.x > 1.0f - k_NomOffset && offset.x <= 1.0f - k_NomOffset;
                break;
        }

        if (nom)
        {
            if (position.GetCell()->GetPill())
            {
                m_AtePill = true;
                m_StopTimer = k_PillStopTime;
                m_PillSound.Play();
            }
            else
            {
                m_AtePowerPill = true;
                m_StopTimer = k_PowerPillStopTime;
                m_PowerPillSound.Play();
            }

            position.GetCell()->Nom();
        }
    }

    m_PreviousOffset = offset;

#if defined _DEBUG
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))
    {
        // Force fright mode
        m_AtePowerPill = true;
    }
#endif
}

void Player::UpdateSprite(float dt)
{
    Direction const direction = m_Movement.GetDirection();
    if (direction != Direction::None)
    {
        float rot = 0.0f;
        switch(direction)
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
    sf::Vector2f const gridPos = m_Movement.GetAbsolutePosition();
    sf::Vector2f const pos = transform.transformPoint(gridPos);
    m_Sprite.SetPosition(pos);
    m_Sprite.Draw(target);
}
