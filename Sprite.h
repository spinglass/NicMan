#pragma once

class Sprite
{
public:
    Sprite();
    virtual ~Sprite();

    void Load(char* filename);
    void Draw(sf::RenderTarget& target);

    void SetOriginToCentre();
    void SetPosition(float x, float y);
    void SetPosition(sf::Vector2f const& pos);

private:
    sf::Texture m_Texture;
    sf::Sprite m_Sprite;
};
