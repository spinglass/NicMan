#include "stdafx.h"
#include "ScoreManager.h"

ScoreManager::ScoreManager() :
    m_Points(0),
    m_Lives(3),
    m_Ready(false)
{
}

ScoreManager::~ScoreManager()
{
}

void ScoreManager::Reset(int lives)
{
    m_Points = 0;
    m_Lives = lives;
    m_Ready = false;
}

void ScoreManager::Add(int points)
{
    m_Points += points;
}

void ScoreManager::LoseLife()
{
    --m_Lives;
}

bool ScoreManager::GetGameOver() const
{
    return (m_Lives < 0);
}