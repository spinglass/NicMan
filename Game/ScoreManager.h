#pragma once

class ScoreManager
{
public:
    ScoreManager();
    virtual ~ScoreManager();

    void Reset(int lives);
    void Add(int points);
    void LoseLife();
    void SetReady(bool ready) { m_Ready = ready; }

    int GetPoints() const { return m_Points; }
    int GetLives() const { return m_Lives; }
    bool GetGameOver() const;
    bool GetReady() const { return m_Ready; }

private:
    ScoreManager(ScoreManager&);
    ScoreManager& operator=(ScoreManager&);

    int m_Points;
    int m_Lives;
    bool m_Ready;
};
