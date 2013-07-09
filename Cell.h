#pragma once

class Cell
{
public:
    enum Type
    {
        Space,
        Wall,
        GhostBase,
    };

    Cell();
    static Cell Parse(char c);

    Type GetType() const { return m_Type; }
    bool GetPill() const { return m_Pill; }
    bool GetPowerPill() const { return m_PowerPill; }

    void Nom();
    void Reset();

private:
    Type m_Type;

    bool m_Pill;
    bool m_WasPill;

    bool m_PowerPill;
    bool m_WasPowerPill;
};
