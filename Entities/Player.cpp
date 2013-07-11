#include "stdafx.h"
#include "Player.h"

Player::Player()
{
}

Player::~Player()
{
}

void Player::Load()
{
    m_Sprite.Load("Resources/player_00.png");
    m_Sprite.SetOriginToCentre();
}

void Player::SetPosition(float x, float y)
{
    m_Position.x = x;
    m_Position.y = y;
}

void Player::Update(float dt)
{
}

void Player::Draw(sf::RenderTarget& target, sf::Transform const& transform)
{
    sf::Vector2f pos = transform.transformPoint(m_Position);
    m_Sprite.SetPosition(pos);
    m_Sprite.Draw(target);
}
