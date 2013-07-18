#include "stdafx.h"
#include "Ghost.h"

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

void Ghost::SelectNextExitDirection()
{
    GridRef nextRef;
    switch(m_NextDirection)
    {
        case Direction::North:
        {
            nextRef = m_GridRef.North();
            assert(nextRef.CanPlayerPass());
            break;
        }
        case Direction::South:
        {
            nextRef = m_GridRef.South();
            assert(nextRef.CanPlayerPass());
            break;
        }
        case Direction::East:
        {
            nextRef = m_GridRef.East();
            assert(nextRef.CanPlayerPass());
            break;
        }
        case Direction::West:
        {
            nextRef = m_GridRef.West();
            assert(nextRef.CanPlayerPass());
            break;
        }
    }

    bool const north = nextRef.North().CanPlayerPass();
    bool const south = nextRef.South().CanPlayerPass();
    bool const east = nextRef.East().CanPlayerPass();
    bool const west = nextRef.West().CanPlayerPass();

    std::vector<Direction> options;

    switch(m_NextDirection)
    {
        case Direction::North:
        {
            if (north)
            {
                options.push_back(Direction::North);
            }
            if (east)
            {
                options.push_back(Direction::East);
            }
            if (west)
            {
                options.push_back(Direction::West);
            }
            break;
        }
        case Direction::South:
        {
            if (south)
            {
                options.push_back(Direction::South);
            }
            if (east)
            {
                options.push_back(Direction::East);
            }
            if (west)
            {
                options.push_back(Direction::West);
            }
            break;
        }
        case Direction::East:
        {
            if (north)
            {
                options.push_back(Direction::North);
            }
            if (east)
            {
                options.push_back(Direction::East);
            }
            if (south)
            {
                options.push_back(Direction::South);
            }
            break;
        }
        case Direction::West:
        {
            if (north)
            {
                options.push_back(Direction::North);
            }
            if (south)
            {
                options.push_back(Direction::South);
            }
            if (west)
            {
                options.push_back(Direction::West);
            }
            break;
        }
    }

    int choice = rand() % options.size();
    m_ExitDirection = options[choice];
}

void Ghost::UpdateMovement(float dt)
{
    GridRef const northRef = m_GridRef.North();
    GridRef const southRef = m_GridRef.South();
    GridRef const eastRef = m_GridRef.East();
    GridRef const westRef = m_GridRef.West();

    if (m_Direction == Direction::None)
    {
        m_Direction = Direction::West;
        m_ExitDirection = Direction::West;
        m_NextDirection = Direction::West;
    }

    Move(m_Direction, dt);

    // Check for next cell or a wall
    bool nextCell = false;
    switch (m_Direction)
    {
        case Direction::North:
        {
            if (m_NextDirection == Direction::North)
            {
                if (m_Offset.y >= 1.0f)
                {
                    // Next cell
                    m_Offset.y -= 1.0f;
                    m_GridRef = northRef;
                    nextCell = true;
                }
            }
            else
            {
                if (m_Offset.y >= 0.5f)
                {
                    // Change direction
                    m_Offset.y = 0.5f;
                    m_Direction = m_NextDirection;
                }
            }
            break;
        }
        case Direction::South:
        {
            if (m_NextDirection == Direction::South)
            {
                if (m_Offset.y <= 0.0f)
                {
                    // Next cell
                    m_Offset.y += 1.0f;
                    m_GridRef = southRef;
                    nextCell = true;
                }
            }
            else
            {
                if (m_Offset.y <= 0.5f)
                {
                    // Change direction
                    m_Offset.y = 0.5f;
                    m_Direction = m_NextDirection;
                }
            }
            break;
        }
        case Direction::East:
        {
            if (m_NextDirection == Direction::East)
            {
                if (m_Offset.x >= 1.0f)
                {
                    // Next cell
                    m_Offset.x -= 1.0f;
                    m_GridRef = eastRef;
                    nextCell = true;
                }
            }
            else
            {
                if (m_Offset.x >= 0.5f)
                {
                    // Stop
                    m_Offset.x = 0.5f;
                    m_Direction = m_NextDirection;
                }
            }
            break;
        }
        case Direction::West:
        {
            if (m_NextDirection == Direction::West)
            {
                if (m_Offset.x < 0.0f)
                {
                    // Next cell
                    m_Offset.x += 1.0f;
                    m_GridRef = westRef;
                    nextCell = true;
                }
            }
            else
            {
                if (m_Offset.x <= 0.5f)
                {
                    // Stop
                    m_Offset.x = 0.5f;
                    m_Direction = m_NextDirection;
                }
            }
            break;
        }
    }

    if (nextCell)
    {
        m_NextDirection = m_ExitDirection;
        SelectNextExitDirection();
    }
}
