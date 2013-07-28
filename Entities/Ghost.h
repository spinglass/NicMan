#pragma once

#include <memory>

#include "Core/Sound.h"
#include "Core/Sprite.h"
#include "Entity.h"
#include "Maze/BaseMovement.h"
#include "Maze/Movement.h"

class IGhostTarget;

class Ghost : public Entity
{
public:
    enum class Behaviour
    {
        None,
        Chase,
        Scatter,
        Fright,
        Eaten,
    };

    Ghost(Maze const& maze);
    virtual ~Ghost();

    void Load(int id);
    void SetHomePosition(BaseMovement::HomePosition homePosition);
    void Restart(GridRef const& ref, float offsetX, float offsetY);
    void RestartInBase();
    void SetTarget(Behaviour behaviour, std::shared_ptr<IGhostTarget> const& target);
    void SetBehaviour(Behaviour behaviour);
    void SetSpeed(float speed);
    void ExitBase();
    void SetFrightFlash(bool flash) { m_FrightFlash = flash; }

    Movement const& GetMovement() const { return m_Movement; }
    Behaviour GetBehaviour() const { return m_Behaviour; }

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Ghost(Ghost&);
    Ghost& operator=(Ghost&);

    Direction SelectExitDirection(GridRef const ref, Direction enterDirection);
    Direction SelectNextDirection();

    std::map<Behaviour, std::shared_ptr<IGhostTarget>> m_Targets;

    Sprite m_Body;
    std::map<Direction, Sprite> m_Eyes;
    Sprite m_Fright;

    Movement m_Movement;
    BaseMovement m_BaseMovement;
    GridRef m_TargetRef;
    Direction m_NextDirection;
    Behaviour m_Behaviour;
    Behaviour m_EatenExitBehaviour;
    bool m_InBase;
    bool m_Reverse;
    bool m_FrightFlash;
};
