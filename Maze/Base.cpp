#include "stdafx.h"
#include "Base.h"

float const Base::k_HalfWidth = 2.0f;
float const Base::k_HalfHeight = 0.5f;
float const Base::k_ExitHeight = 3.0f;

Base::Base() :
    ExitX(0.0f),
    ExitY(0.0f),
    Top(0.0f),
    Bottom(0.0f),
    Left(0.0f),
    Right(0.0)
{
}

void Base::Load(tinyxml2::XMLElement const& element)
{
    ExitX = element.FloatAttribute("ExitX");
    ExitY = element.FloatAttribute("ExitY");

    Middle = ExitY - k_ExitHeight;
    Top = Middle + k_HalfHeight;
    Bottom = Middle - k_HalfHeight;
    Left = ExitX - k_HalfWidth;
    Right = ExitX + k_HalfWidth;
}