#pragma once

#include "Core/Sprite.h"
#include "Entity.h"

class Fruit : public Entity
{
public:
    Fruit();
    virtual ~Fruit();

    bool IsVisible() const { return m_Visible; }
    sf::Vector2i GetPosition() const;
    int GetScore() const { return m_Score; }

    void Load(tinyxml2::XMLElement const& element);
    void SetPosition(sf::Vector2f pos) { m_Position = pos; }
    void Show();
    void Nom();
    void NomPill();

    virtual void Update(float dt) override;
    virtual void Draw(sf::RenderTarget& target, sf::Transform const& transform) override;

private:
    Sprite m_Sprite;
    sf::Vector2f m_Position;
    int m_Score;
    int m_Appear1;
    int m_Appear2;
    float m_Time;

    int m_PillCount;
    float m_Timer;
    bool m_Visible;
};
