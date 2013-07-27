#include "stdafx.h"
#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite() :
    m_Current(0),
    m_FPS(30.0f),
    m_FrameTime(0.0f),
    m_Playing(false),
    m_Loop(false),
    m_Shuttle(false),
    m_ShuttleUp(true)
{
}

AnimatedSprite::~AnimatedSprite()
{
}

void AnimatedSprite::Load(char* filename)
{
    m_Textures.clear();

    bool loaded;
    do
    {
        char fullFilename[FILENAME_MAX];
        sprintf_s(fullFilename, "%s_%02d.png", filename, m_Textures.size());

        sf::Texture tmp;
        loaded = tmp.loadFromFile(fullFilename);

        if (loaded)
        {
            m_Textures.push_back(tmp);
        }

    } while (loaded);

    if (m_Textures.size() > 0)
    {
        m_Sprite.setTexture(m_Textures[0], true);
    }
}

void AnimatedSprite::Reset()
{
    m_FrameTime = 0.0f;
    m_Playing = false;
    m_Current = 0;
    m_ShuttleUp = true;
    m_Sprite.setTexture(m_Textures[m_Current], false);
}

void AnimatedSprite::Play()
{
    m_Playing = true;
}

void AnimatedSprite::Pause()
{
    m_Playing = false;
}

void AnimatedSprite::Update(float dt)
{
    if (m_Playing && m_Textures.size() > 0)
    {
        m_FrameTime += dt;
        if (m_FrameTime > 1.0f / m_FPS)
        {
            m_FrameTime = 0.0f;

            if (m_Loop)
            {
                m_Current = (m_Current + 1) % m_Textures.size();
            }
            else if (m_Shuttle)
            {
                if (m_ShuttleUp)
                {
                    ++m_Current;
                    if (m_Current == m_Textures.size() - 1)
                    {
                        m_ShuttleUp = false;
                    }
                }
                else
                {
                    --m_Current;
                    if (m_Current == 0)
                    {
                        m_ShuttleUp = true;
                    }
                }
            }
            else
            {
                ++m_Current;
                if (m_Current == m_Textures.size())
                {
                    --m_Current;
                    m_Playing = false;
                }
            }
            m_Sprite.setTexture(m_Textures[m_Current], false);
        }
    }
}

void AnimatedSprite::Draw(sf::RenderTarget& target)
{
    target.draw(m_Sprite);
}

void AnimatedSprite::SetOriginToCentre()
{
    sf::Vector2u size = m_Textures[0].getSize();
    sf::Vector2f origin(0.5f * size.x, 0.5f * size.y);
    m_Sprite.setOrigin(origin);
}

void AnimatedSprite::SetPosition(float x, float y)
{
    m_Sprite.setPosition(x, y);
}

void AnimatedSprite::SetPosition(sf::Vector2f const& pos)
{
    m_Sprite.setPosition(pos);
}

void AnimatedSprite::SetRotation(float degs)
{
    m_Sprite.setRotation(degs);
}