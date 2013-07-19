#pragma once

#include "Sprite.h"
#include "Entity.h"
#include "Maze/GridRef.h"
#include "Sound.h"

class Ghost : public Entity
{
public:
    Ghost();
    virtual ~Ghost();

    void Load();
    void SetPosition(GridRef const& ref, float offsetX, float offsetY);

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    void Move(Direction dir, float dt);

    void UpdateMovement(float dt);
    void SelectNextDirection();

    Sprite m_Sprite;
    GridRef m_GridRef;
    sf::Vector2f m_Offset;
    Direction m_Direction;
    Direction m_ExitDirection;
    Direction m_NextDirection;
};
