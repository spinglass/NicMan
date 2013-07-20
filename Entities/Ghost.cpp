#include "stdafx.h"
#include "Ghost.h"

#include "Maze/Direction.h"
#include "GhostTargets\IGhostTarget.h"

Ghost::Ghost() :
    m_Movement(false),
    m_NextDirection(Direction::None),
    m_Behaviour(Behaviour::Scatter),
    m_PendingBehaviour(Behaviour::None)
{
    Movement::NextCellFunc func = [this]() { return this->SelectNextDirection(); };
    m_Movement.SetNextCellFunc(func);
}

Ghost::~Ghost()
{
}

void Ghost::Load(int id)
{
    m_Movement.SetSpeed(7.5f);

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
    m_Movement.Reset(ref, offsetX, offsetY);
    m_Movement.SetDirection(Direction::West);
    m_Movement.SetExitDirection(Direction::West);
    m_Behaviour = Behaviour::Scatter;
    m_PendingBehaviour = Behaviour::None;
    SelectNextDirection();
}

void Ghost::SetTarget(Behaviour behaviour, std::shared_ptr<IGhostTarget> const& target)
{
    m_Targets[behaviour] = target;
}

void Ghost::SetBehaviour(Behaviour behaviour)
{
    if (m_Behaviour != behaviour)
    {
        m_PendingBehaviour = behaviour;
    }
}

void Ghost::Update(float dt)
{
    m_Movement.Update(dt);
}

void Ghost::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    static bool k_DebugDraw = false;

    sf::Vector2f const gridPos = m_Movement.GetAbsolutePosition();
    sf::Vector2f pos = transform.transformPoint(gridPos);
    m_Body.SetPosition(pos);
    m_Body.Draw(target);
    Sprite& eyes = m_Eyes[m_Movement.GetDirection()];
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

Direction Ghost::SelectNextDirection()
{
    if (m_PendingBehaviour != Behaviour::None)
    {
        assert(m_PendingBehaviour != m_Behaviour);
        m_Behaviour = m_PendingBehaviour;
        m_PendingBehaviour = Behaviour::None;

        // Reverse.
        // Just left a cell, so immediately re-enter it.
        Direction const reverseDirection = Opposite(m_Movement.GetDirection());
        m_Movement.SetDirection(reverseDirection);
        m_NextDirection = reverseDirection;
    }

    // Update target position
    assert(m_Targets[m_Behaviour]);
    m_TargetRef = m_Targets[m_Behaviour]->It();

    GridRef const& position = m_Movement.GetPosition();
    Direction const nextExitDirection = m_NextDirection;
    Direction const nextEnterDirection = Opposite(nextExitDirection);

    // Get the next cell we'll be entering, where the decision is required for exit.
    GridRef const nextRef = position.GetNext(nextExitDirection);
    assert(nextRef.CanPlayerPass());

    // Find possible exit direction from the next cell.
    // Search order set for preferred direction in case of two equal options.
    Direction const searchOrder[] = { Direction::North, Direction::West, Direction::South, Direction::East };
    Direction nextDirection = Direction::None;
    float minDistSq = FLT_MAX;
    for (Direction dir : searchOrder)
    {
        // If it's not back the way we came...
        if (dir != nextEnterDirection)
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

    std::swap(m_NextDirection, nextDirection);
    return nextDirection;
}
