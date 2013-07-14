#pragma once

#include "Entity.h"
#include "GridRef.h"
#include "Sprite.h"

class Player : public Entity
{
public:
    Player();
    virtual ~Player();

    void Load();
    void SetPosition(GridRef const& ref, float offsetX, float offsetY);

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    enum class Direction
    {
        None,
        North,
        East,
        South,
        West,
    };

    void UpdateMovement(float dt);
    void Move(Direction dir, float dt);

    Sprite m_Sprite;
    GridRef m_GridRef;
    sf::Vector2f m_Offset;
    Direction m_Direction;
    Direction m_NextDirection;
    sf::Vector2f m_Speed;
};
