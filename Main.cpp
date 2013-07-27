// NicMan.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Game/Game.h"

int main()
{
    SetCurrentDirectory(L"Resources");

    {
        Game game;
        game.Run();
    }

	return 0;
}

