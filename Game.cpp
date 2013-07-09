#include "stdafx.h"
#include "Game.h"

#include "Level.h"

Game::Game()
{
    m_Window = new sf::RenderWindow(sf::VideoMode(1080, 720), "Nic-Man", sf::Style::Close);
    m_Level = new Level();
    m_Level->Load("Resources/level01.txt");
}

Game::~Game()
{
    m_Window->close();
    delete m_Window;
}

void Game::Run()
{
    bool running = true;
    while (running)
    {
        sf::Event event;
        while (m_Window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed
                || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
            {
                running = false;
            }
        }

        m_Window->clear();
        m_Level->Draw(*m_Window);
        m_Window->display();
    }
}
