#include "stdafx.h"
#include "Ghost.h"

#include "Maze/Direction.h"
#include "GhostTargets\IGhostTarget.h"

Ghost::Ghost() :
    m_Direction(Direction::None),
    m_ExitDirection(Direction::None),
    m_NextDirection(Direction::None)
{
}

Ghost::~Ghost()
{
}

void Ghost::Load(int id)
{
    char bodyFilename[FILENAME_MAX];
    sprintf_s(bodyFilename, "Resources/ghost_body_%02d", id);
    m_Body.Load(bodyFilename);
    m_Body.SetOriginToCentre();

    m_Eyes[Direction::North].Load("Resources/ghost_eyes_north");
    m_Eyes[Direction::North].SetOriginToCentre();
    m_Eyes[Direction::South].Load("Resources/ghost_eyes_south");
    m_Eyes[Direction::South].SetOriginToCentre();
    m_Eyes[Direction::East].Load("Resources/ghost_eyes_east");
    m_Eyes[Direction::East].SetOriginToCentre();
    m_Eyes[Direction::West].Load("Resources/ghost_eyes_west");
    m_Eyes[Direction::West].SetOriginToCentre();
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
    static bool k_DebugDraw = false;

    sf::Vector2f gridPos = sf::Vector2f((float)m_GridRef.X(), (float)m_GridRef.Y()) + m_Offset;
    sf::Vector2f pos = transform.transformPoint(gridPos);
    m_Body.SetPosition(pos);
    m_Body.Draw(target);
    Sprite& eyes = m_Eyes[m_Direction];
    eyes.SetPosition(pos);
    eyes.Draw(target);

    if (k_DebugDraw)
    {
        sf::Vector2f targetGridPos = sf::Vector2f((float)m_TargetRef.X(), (float)m_TargetRef.Y()) + sf::Vector2f(0.5f, 0.5f);
        sf::Vector2f targetPos = transform.transformPoint(targetGridPos);
        sf::VertexArray lines(sf::LinesStrip, 2);
        lines[0].position = pos;
        lines[0].color = sf::Color::White;
        lines[1].position = targetPos;
        lines[1].color = sf::Color::White;
        target.draw(lines);
    }
}

void Ghost::Move(Direction dir, float dt)
{
    float const k_Speed = 7.5f;
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
    assert(m_Target);
    m_TargetRef = m_Target->It();

    // Get the next cell we'll be entering, where the decision is required for exit.
    GridRef const nextRef = m_GridRef.GetNext(m_ExitDirection);
    assert(nextRef.CanPlayerPass());

    // Find possible exit direction from the next cell.
    // Search order set for preferred direction in case of two equal options.
    Direction const searchOrder[] = { Direction::North, Direction::West, Direction::South, Direction::East };
    Direction const enterDirection = Opposite(m_ExitDirection);
    Direction nextDirection = Direction::None;
    float minDistSq = FLT_MAX;
    for (Direction dir : searchOrder)
    {
        // If it's not back the way we came...
        if (dir != enterDirection)
        {
            // ...and we can pass, it's an option
            GridRef const optionRef = nextRef.GetNext(dir);
            if (optionRef.CanPlayerPass())
            {
                // Check the distance to the target
                float const distSq = DistanceSq(m_TargetRef, optionRef);
                if (distSq < minDistSq)
                {
                    nextDirection = dir;
                    minDistSq = distSq;
                }
            }
        }
    }
    assert(nextDirection != Direction::None);
    m_NextDirection = nextDirection;
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
