#include "stdafx.h"
#include "Ghost.h"

#include "Maze/Direction.h"
#include "Maze/Maze.h"
#include "GhostTargets\IGhostTarget.h"

Ghost::Ghost(Maze const& maze) :
    m_Movement(maze, false),
    m_BaseMovement(maze.GetBase(), m_Movement),
    m_NextDirection(Direction::None),
    m_Behaviour(Behaviour::Scatter),
    m_EatenExitBehaviour(Behaviour::None),
    m_ExitTimer(0.0f),
    m_Reverse(false),
    m_FrightFlash(false)
{
    Movement::NextCellFunc func = [this]() { return this->SelectNextDirection(); };
    m_Movement.SetNextCellFunc(func);
}

Ghost::~Ghost()
{
}

void Ghost::Load(int id)
{
    char bodyFilename[FILENAME_MAX];
    sprintf_s(bodyFilename, "Ghosts/Ghost_body_%02d", id);
    m_Body.Load(bodyFilename);
    m_Body.SetOriginToCentre();

    m_Eyes[Direction::North].Load("Ghosts/Ghost_eyes_north");
    m_Eyes[Direction::North].SetOriginToCentre();
    m_Eyes[Direction::South].Load("Ghosts/Ghost_eyes_south");
    m_Eyes[Direction::South].SetOriginToCentre();
    m_Eyes[Direction::East].Load("Ghosts/Ghost_eyes_east");
    m_Eyes[Direction::East].SetOriginToCentre();
    m_Eyes[Direction::West].Load("Ghosts/Ghost_eyes_west");
    m_Eyes[Direction::West].SetOriginToCentre();

    m_Fright.Load("Ghosts/Ghost_fright");
    m_Fright.SetOriginToCentre();
}

void Ghost::SetHomePosition(BaseMovement::HomePosition homePosition)
{
    m_BaseMovement.SetHomePosition(homePosition);
}

void Ghost::Restart(float x, float y)
{
    m_Movement.Reset(x, y);
    m_Movement.SetDirection(Direction::West);
    m_Movement.SetExitDirection(Direction::West);
    m_NextDirection = Direction::West;

    m_Behaviour = Behaviour::Scatter;
    m_EatenExitBehaviour = Behaviour::None;
    m_BaseMovement.ResetToOut();
    m_Reverse = false;
}

void Ghost::RestartInBase()
{
    m_BaseMovement.ResetToHome();
}

void Ghost::SetTarget(Behaviour behaviour, std::shared_ptr<IGhostTarget> const& target)
{
    m_Targets[behaviour] = target;
}

void Ghost::SetBehaviour(Behaviour behaviour)
{
    if (behaviour != m_Behaviour)
    {
        switch(m_Behaviour)
        {
            case Behaviour::Chase:
            case Behaviour::Scatter:
                assert(behaviour != Behaviour::Eaten);
                if (behaviour == Behaviour::Fright)
                {
                    m_EatenExitBehaviour = m_Behaviour;
                }
                m_Behaviour = behaviour;
                m_Reverse = true;
                break;
            case Behaviour::Fright:
                // Straight into other behaviour
                m_Behaviour = behaviour;
                break;
            case Behaviour::Eaten:
                if (behaviour == Behaviour::Scatter || m_Behaviour == Behaviour::Chase)
                {
                    // Main behaviour has changed - enter it once no longer eaten
                    m_EatenExitBehaviour = behaviour;
                }
                break;
        }
    }
}

void Ghost::SetSpeed(float speed)
{
    m_Movement.SetSpeed(speed);
}

void Ghost::Update(float dt)
{
    // Update initial base exit
    if (m_ExitTimer >= 0.0f)
    {
        m_ExitTimer -= dt;
        if (m_ExitTimer < 0.0f)
        {
            m_BaseMovement.Exit();
        }
    }

    bool const outOfBase = m_BaseMovement.IsOut();
    if (outOfBase)
    {
        // Only update normal movement when out of base
        m_Movement.Update(dt);

        if (m_Behaviour == Behaviour::Eaten)
        {
            // Returning to base - set base movement to capture when back
            m_BaseMovement.Capture();
        }
    }
    else
    {
        if (m_BaseMovement.IsHome() && m_Behaviour == Behaviour::Eaten)
        {
            // Immediately exit when returned home having been eaten
            m_BaseMovement.Exit();
            m_Behaviour = m_EatenExitBehaviour;
        }
    }

    // Alway update base movement - might be in capture mode
    m_BaseMovement.Update(dt);

    if (!outOfBase && m_BaseMovement.IsOut())
    {
        // Just left base - start normal movement
        m_Movement.SetDirection(Direction::West);
        m_Movement.SetExitDirection(Direction::West);
        m_NextDirection = Direction::West;
        m_Reverse = false;
    }
}

void Ghost::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    static bool k_DebugDraw = false;

    sf::Vector2f const gridPos = m_Movement.GetAbsolutePosition();
    sf::Vector2f pos = transform.transformPoint(gridPos);

    bool const showEasten = (m_Behaviour == Behaviour::Eaten);
    bool const showFright = (m_Behaviour == Behaviour::Fright && !m_FrightFlash);

    if (!showFright && !showEasten)
    {
        m_Body.SetPosition(pos);
        m_Body.Draw(target);
    }

    if (!showFright)
    {
        Sprite& eyes = m_Eyes[m_Movement.GetDirection()];
        eyes.SetPosition(pos);
        eyes.Draw(target);
    }

    if (showFright)
    {
        m_Fright.SetPosition(pos);
        m_Fright.Draw(target);
    }

    if (k_DebugDraw)
    {
        if (m_Behaviour != Behaviour::Fright)
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
}

Direction Ghost::SelectExitDirection(GridRef const ref, Direction const enterDirection)
{
    assert(ref.CanPlayerPass());

    // Can't exit the way we came in
    Direction const noExitDirection = Opposite(enterDirection);

    // Build set of available options
    // Search order set for preferred direction in case of two equal options.
    Direction const searchOrder[] = { Direction::North, Direction::West, Direction::South, Direction::East };
    std::vector<std::pair<Direction, GridRef>> options;
    for (Direction dir : searchOrder)
    {
        // If it's not back the way we came...
        if (dir != noExitDirection)
        {
            // ...and we can pass, it's an option
            GridRef const optionRef = ref.GetNext(dir);
            if (optionRef.CanPlayerPass())
            {
                options.push_back(std::make_pair(dir, optionRef));
            }
        }
    }

    Direction exitDirection = Direction::None;
    if (m_Behaviour == Behaviour::Fright)
    {
        // Randomly select an option
        int const ran = rand() % options.size();
        exitDirection = options[ran].first;
    }
    else
    {
        // Update target position
        assert(m_Targets[m_Behaviour]);

        m_TargetRef = m_Targets[m_Behaviour]->It();

        // Find possible exit direction from the next cell.
        float minDistSq = FLT_MAX;
        for (auto option : options)
        {
            // Check the distance to the target
            float const distSq = DistanceSq(m_TargetRef, option.second);
            if (distSq < minDistSq)
            {
                exitDirection = option.first;
                minDistSq = distSq;
            }
        }
    }
    assert(exitDirection != Direction::None);

    return exitDirection;
}

Direction Ghost::SelectNextDirection()
{
    if (m_Reverse)
    {
        // Just left a cell, so immediately re-enter it.
        Direction const reverseDirection = Opposite(m_Movement.GetDirection());
        m_Movement.SetDirection(reverseDirection);
        m_Movement.SetExitDirection(reverseDirection);
        m_NextDirection = reverseDirection;
        m_Reverse = false;
    }

    // Get the next cell we'll be entering, where the decision is required for exit.
    GridRef const nextRef = m_Movement.GetPosition().GetNext(m_NextDirection);

    // And select the exit direction from that cell
    Direction nextDirection = SelectExitDirection(nextRef, m_NextDirection);

    // Return the previously selected next direction and save the new selected direction for next time
    std::swap(m_NextDirection, nextDirection);
    return nextDirection;
}
