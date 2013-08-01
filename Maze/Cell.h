#pragma once

class Cell
{
public:
    Cell(char c);

    static bool IsCell(char c);

    bool GetPill() const { return m_Pill; }
    bool GetPowerPill() const { return m_PowerPill; }
    bool IsGhostBase() const { return m_GhostBase; }
    bool IsTunnel() const { return m_Tunnel; }
    int GetWarpId() const { return m_WarpId; }

    void Nom();
    void Reset();

private:
    void Parse(char c);

    bool m_Pill;
    bool m_WasPill;

    bool m_PowerPill;
    bool m_WasPowerPill;

    bool m_GhostBase;
    bool m_Tunnel;
    int m_WarpId;
};
