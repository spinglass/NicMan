#pragma once

#include "AnimatedSprite.h"
#include "Entity.h"
#include "Maze/Movement.h"
#include "Sound.h"

class Player : public Entity
{
public:
    Player();
    virtual ~Player();

    void Load();
    void SetPosition(GridRef const& ref, float offsetX, float offsetY);
    GridRef const& GetPosition() const { return m_Movement.GetPosition(); }
    Direction GetDirection() const { return m_Movement.GetDirection(); }

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Player(Player&);
    Player& operator=(Player&);

    void UpdateControls();
    void UpdateNomming();
    void UpdateSprite(float dt);

    AnimatedSprite m_Sprite;
    Movement m_Movement;
    sf::Vector2f m_PreviousOffset;
    float m_StopTimer;

    Sound m_PillSound;
    Sound m_PowerPillSound;
};
