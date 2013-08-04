#include "stdafx.h"
#include "Fruit.h"

Fruit::Fruit() :
    m_Score(0),
    m_Appear1(0),
    m_Appear2(0),
    m_Time(0.0f),
    m_Timer(0.0f),
    m_PillCount(0),
    m_Visible(false)
{
}

Fruit::~Fruit()
{
}

void Fruit::Load(tinyxml2::XMLElement const& element)
{
    char const* sprite = element.Attribute("Sprite");
    assert(sprite);
    m_Sprite.Load(sprite);
    m_Sprite.SetOriginToCentre();

    m_Score = element.IntAttribute("Score");
    m_Appear1 = element.IntAttribute("Appear1");
    m_Appear2 = element.IntAttribute("Appear2");
    m_Time = element.FloatAttribute("Time");
}

sf::Vector2i Fruit::GetPosition() const
{
    return sf::Vector2i((int)m_Position.x, (int)m_Position.y);
}

void Fruit::Show()
{
    m_Visible = true;
}

void Fruit::Nom()
{
    m_Visible = false;
}

void Fruit::NomPill()
{
    ++m_PillCount;
    if (m_PillCount == m_Appear1 || m_PillCount == m_Appear2)
    {
        m_Visible = true;
        m_Timer = m_Time;
    }
}

void Fruit::Update(float dt)
{
    m_Timer -= dt;
    if (m_Timer < 0.0f)
    {
        m_Visible = false;
    }
}

void Fruit::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    if (m_Visible)
    {
        sf::Vector2f const pos = transform.transformPoint(m_Position);
        m_Sprite.SetPosition(pos);
        m_Sprite.Draw(target);
    }
}
