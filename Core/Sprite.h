#pragma once

class Sprite
{
public:
    Sprite();
    virtual ~Sprite();

    void Load(char const* filename);
    void Draw(sf::RenderTarget& target);

    void SetOriginToCentre();
    void SetPosition(float x, float y);
    void SetPosition(sf::Vector2f const& pos);
    void SetColor(sf::Color color);

private:
    sf::Texture m_Texture;
    sf::Sprite m_Sprite;
};
