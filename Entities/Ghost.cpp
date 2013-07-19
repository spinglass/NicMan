#include "stdafx.h"
#include "Ghost.h"

#include "Maze/Direction.h"

Ghost::Ghost() :
    m_Direction(Direction::None),
    m_ExitDirection(Direction::None),
    m_NextDirection(Direction::None)
{
}

Ghost::~Ghost()
{
}

void Ghost::Load()
{
    m_Sprite.Load("Resources/ghost_00_body");
    m_Sprite.SetOriginToCentre();
}

void Ghost::SetPosition(GridRef const& ref, float offsetX, float offsetY)
{
    m_GridRef = ref;
    m_Offset.x = offsetX;
    m_Offset.y = offsetY;
}


void Ghost::Update(float dt)
{
    UpdateMovement(dt);
}

void Ghost::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f gridPos = sf::Vector2f((float)m_GridRef.X(), (float)m_GridRef.Y()) + m_Offset;
    sf::Vector2f pos = transform.transformPoint(gridPos);
    m_Sprite.SetPosition(pos);
    m_Sprite.Draw(target);
}

void Ghost::Move(Direction dir, float dt)
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

void Ghost::SelectNextDirection()
{
    // Get the next cell we'll be entering, where the decision is required for exit.
    GridRef nextRef = m_GridRef.GetNext(m_ExitDirection);
    assert(nextRef.CanPlayerPass());

    // Find possible exit direction from the next cell
    std::vector<Direction> exitOptions;
    Direction const enterDirection = Opposite(m_ExitDirection);
    for (DirectionIter iter = DirectionIter::Begin(); iter != DirectionIter::End(); ++iter)
    {
        // If it's not back the way we came and we can pass, it's an option
        if (*iter != enterDirection && nextRef.GetNext(*iter).CanPlayerPass())
        {
            exitOptions.push_back(*iter);
        }
    }
    assert(exitOptions.size() > 0);

    // Randomly choose one
    int const choice = rand() % exitOptions.size();
    m_NextDirection = exitOptions[choice];
}

void Ghost::UpdateMovement(float dt)
{
    if (m_Direction == Direction::None)
    {
        m_Direction = Direction::West;
        m_ExitDirection = Direction::West;
    }
    
    if (m_NextDirection == Direction::None)
    {
        SelectNextDirection();
    }

    Move(m_Direction, dt);

    // Check for next cell or a wall
    bool nextCell = false;
    switch (m_Direction)
    {
        case Direction::North:
        {
            if (m_ExitDirection == Direction::North)
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
                    // Change direction
                    m_Offset.y = 0.5f;
                    m_Direction = m_ExitDirection;
                }
            }
            break;
        }
        case Direction::South:
        {
            if (m_ExitDirection == Direction::South)
            {
                if (m_Offset.y <= 0.0f)
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
                    // Change direction
                    m_Offset.y = 0.5f;
                    m_Direction = m_ExitDirection;
                }
            }
            break;
        }
        case Direction::East:
        {
            if (m_ExitDirection == Direction::East)
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
                    // Stop
                    m_Offset.x = 0.5f;
                    m_Direction = m_ExitDirection;
                }
            }
            break;
        }
        case Direction::West:
        {
            if (m_ExitDirection == Direction::West)
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
                    // Stop
                    m_Offset.x = 0.5f;
                    m_Direction = m_ExitDirection;
                }
            }
            break;
        }
    }

    if (nextCell)
    {
        m_GridRef.Move(m_Direction);
        m_ExitDirection = m_NextDirection;
        m_NextDirection = Direction::None;
    }
}
