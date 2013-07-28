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

    bool IsOut() const;
    bool IsHome() const;

    void SetHomePosition(HomePosition homePosition) { m_HomePosition = homePosition; }
    void ResetToHome();
    void ResetToOut();
    void Exit();
    void Capture();

    void Update(float dt);

private:
    enum class State
    {
        Home,
        Exit,
        Out,
        Enter,
    };

    BaseMovement(BaseMovement&);
    BaseMovement& operator=(BaseMovement&);

    void UpdateHome(float dt);
    void UpdateExit(float dt);
    void UpdateCapture();
    void UpdateEnter(float dt);

    static float const k_HomeSpeed;
    static float const k_ExitSpeed;
    static float const k_EnterSpeed;

    Base const& m_Base;
    Movement& m_Movement;
    HomePosition m_HomePosition;
    State m_State;
    bool m_Capture;
    sf::Vector2f m_PreviousPosition;
};
