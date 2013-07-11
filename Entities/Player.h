#pragma once

#include "Entity.h"
#include "Sprite.h"

class Player : public Entity
{
public:
    Player();
    virtual ~Player();

    void Load();
    void SetPosition(float x, float y);

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Sprite m_Sprite;
    sf::Vector2f m_Position;
};
