#pragma once

class AnimatedSprite
{
public:
    AnimatedSprite();
    virtual ~AnimatedSprite();

    void Load(char* filename);

    void Update(float dt);
    void Draw(sf::RenderTarget& target);

    void SetOriginToCentre();
    void SetPosition(float x, float y);
    void SetPosition(sf::Vector2f const& pos);
    void SetRotation(float degs);

    void Play();
    void Pause();
    void SetSpeed(float fps) { m_FPS = fps; }
    void SetLoop(bool loop) { m_Loop = loop; }
    void SetShuttle(bool shuttle) { m_Shuttle = shuttle; }

private:
    std::vector<sf::Texture> m_Textures;
    sf::Sprite m_Sprite;
    int m_Current;
    float m_FPS;
    float m_FrameTime;
    bool m_Playing;
    bool m_Loop;
    bool m_Shuttle;
    bool m_ShuttleUp;
};
