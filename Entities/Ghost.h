#pragma once

#include <memory>

#include "Sprite.h"
#include "Entity.h"
#include "Maze/Movement.h"
#include "Sound.h"

class IGhostTarget;

class Ghost : public Entity
{
public:
    enum class Behaviour
    {
        None,
        Chase,
        Scatter,
    };

    Ghost();
    virtual ~Ghost();

    void Load(int id);
    void SetPosition(GridRef const& ref, float offsetX, float offsetY);
    void SetTarget(Behaviour behaviour, std::shared_ptr<IGhostTarget> const& target);
    void SetBehaviour(Behaviour behaviour);

    Movement const& GetMovement() const { return m_Movement; }

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Ghost(Ghost&);
    Ghost& operator=(Ghost&);

    Direction SelectNextDirection();

    Sprite m_Body;
    std::map<Direction, Sprite> m_Eyes;
    Movement m_Movement;
    GridRef m_TargetRef;
    Direction m_NextDirection;
    Behaviour m_Behaviour;
    Behaviour m_PendingBehaviour;
    std::map<Behaviour, std::shared_ptr<IGhostTarget>> m_Targets;
};
