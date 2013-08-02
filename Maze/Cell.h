#pragma once

enum class Direction;

class Cell
{
public:
    Cell(char c, int x, int y);

    static bool IsCell(char c);

    int X() const { return m_X; }
    int Y() const { return m_Y; }

    bool GetPill() const { return m_Pill; }
    bool GetPowerPill() const { return m_PowerPill; }
    bool IsOpen() const { return m_Open; }
    bool IsTunnel() const { return m_Tunnel; }

    Cell const* North() const { return m_North; }
    Cell const* East() const { return m_East; }
    Cell const* South() const { return m_South; }
    Cell const* West() const { return m_West; }
    Cell const* GetNext(Direction dir) const;

    void SetNext(Direction dir, Cell const* next);

    void Nom();
    void Reset(); 

private:
    void Parse(char c);

    int m_X;
    int m_Y;

    Cell const* m_North;
    Cell const* m_East;
    Cell const* m_South;
    Cell const* m_West;

    bool m_Pill;
    bool m_WasPill;

    bool m_PowerPill;
    bool m_WasPowerPill;

    bool m_Open;
    bool m_Tunnel;
};
