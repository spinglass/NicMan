#pragma once

#include "Base.h"

class Maze;
class Movement;

class BaseMovement
{
public:
    enum class HomePosition
    {
        Left,
        Middle,
        Right,
    };

    BaseMovement(Base const& base, Movement& movement);
    virtual ~BaseMovement();

    bool IsOut() const { return m_State == State::Out; }

    void SetHomePosition(HomePosition homePosition) { m_HomePosition = homePosition; }
    void ResetToHome();
    void Exit();

    void Update(float dt);

private:
    enum class State
    {
        HomeUp,
        HomeDown,
        Exit,
        Out,
    };

    BaseMovement(BaseMovement&);
    BaseMovement& operator=(BaseMovement&);

    static float const k_Speed;

    Base const& m_Base;
    Movement& m_Movement;
    HomePosition m_HomePosition;
    State m_State;
};
