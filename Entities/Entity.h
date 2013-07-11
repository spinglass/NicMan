#pragma once

class Entity
{
public:
    Entity();
    virtual ~Entity();

    virtual void Update(float dt) = 0;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) = 0;

private:

};
