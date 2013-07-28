#pragma once

struct Base
{
    Base();
    void Load(tinyxml2::XMLElement const& element);

    static float const k_HalfWidth;
    static float const k_HalfHeight;
    static float const k_ExitHeight;

    float ExitX;
    float ExitY;
    float Top;
    float Middle;
    float Bottom;
    float Left;
    float Right;
};
