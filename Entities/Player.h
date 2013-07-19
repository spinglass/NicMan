#pragma once

#include "AnimatedSprite.h"
#include "Entity.h"
#include "Maze/GridRef.h"
#include "Sound.h"

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
    void Move(Direction dir, float dt);

    void UpdateMovement(float dt);
    void UpdateNomming();
    void UpdateSprite(float dt);

    AnimatedSprite m_Sprite;
    GridRef m_GridRef;
    sf::Vector2f m_Offset;
    sf::Vector2f m_PreviousOffset;
    Direction m_Direction;
    Direction m_NextDirection;
    sf::Vector2f m_Speed;

    Sound m_PillSound;
    Sound m_PowerPillSound;
};
