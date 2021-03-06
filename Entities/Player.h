#pragma once

#include "Core/AnimatedSprite.h"
#include "Core/Sound.h"
#include "Entity.h"
#include "Maze/Movement.h"

class Player : public Entity
{
public:
    Player(Maze const& maze);
    virtual ~Player();

    void Load();
    void Restart(float x, float y);

    Movement const& GetMovement() const { return m_Movement; }
    bool AtePill() const { return m_AtePill; }
    bool AtePowerPill() const { return m_AtePowerPill; }
    
    void SetSpeed(float speed);

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
    
    bool m_AtePill;
    bool m_AtePowerPill;

    Sound m_PillSound;
    Sound m_PowerPillSound;
};
