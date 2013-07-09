// NicMan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Game.h"

int CALLBACK WinMain(
    _In_ HINSTANCE hInstance,
    _In_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nCmdShow)
{
    {
        Game game;
        game.Run();
    }

	return 0;
}

