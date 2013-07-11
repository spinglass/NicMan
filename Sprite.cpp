#include "stdafx.h"
#include "Sprite.h"

Sprite::Sprite()
{
}

Sprite::~Sprite()
{
}

void Sprite::Load(char* filename)
{
    if (m_Texture.loadFromFile(filename))
    {
        m_Sprite.setTexture(m_Texture, true);
    }
}

void Sprite::Draw(sf::RenderTarget& target)
{
    target.draw(m_Sprite);
}

void Sprite::SetOriginToCentre()
{
    sf::Vector2u size = m_Texture.getSize();
    sf::Vector2f origin(0.5f * size.x, 0.5f * size.y);
    m_Sprite.setOrigin(origin);
}

void Sprite::SetPosition(float x, float y)
{
    m_Sprite.setPosition(x, y);
}

void Sprite::SetPosition(sf::Vector2f const& pos)
{
    m_Sprite.setPosition(pos);
}
