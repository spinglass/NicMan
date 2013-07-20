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
    Ghost();
    virtual ~Ghost();

    void Load(int id);
    void SetPosition(GridRef const& ref, float offsetX, float offsetY);
    void SetTarget(std::shared_ptr<IGhostTarget> const& target) { m_Target = target; }

    GridRef const& GetPosition() const { return m_Movement.GetPosition(); }

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Ghost(Ghost&);
    Ghost& operator=(Ghost&);

    Direction SelectNextDirection();

    std::shared_ptr<IGhostTarget> m_Target;

    Sprite m_Body;
    std::map<Direction, Sprite> m_Eyes;
    Movement m_Movement;
    GridRef m_TargetRef;
    Direction m_NextDirection;
};
