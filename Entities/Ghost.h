#pragma once

#include <memory>

#include "Sprite.h"
#include "Entity.h"
#include "Maze/GridRef.h"
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

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    void Move(Direction dir, float dt);

    void UpdateMovement(float dt);
    void SelectNextDirection();

    std::shared_ptr<IGhostTarget> m_Target;

    Sprite m_Body;
    std::map<Direction, Sprite> m_Eyes;
    GridRef m_GridRef;
    sf::Vector2f m_Offset;
    Direction m_Direction;
    Direction m_ExitDirection;
    Direction m_NextDirection;
};
