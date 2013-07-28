#include "stdafx.h"
#include "Game.h"

#include "Hud.h"
#include "GlobalSettings.h"
#include "Level.h"
#include "ScoreManager.h"

Game::Game() :
    m_LevelMax(3)
{
    m_Window = new sf::RenderWindow(sf::VideoMode(1080, 720), "Nic-Man", sf::Style::Close);

    m_ScoreManager = new ScoreManager();

    m_Hud = new Hud(*m_ScoreManager);

    Load();
    LoadNextLevel();
}

Game::~Game()
{
    delete m_Hud;
    delete m_ScoreManager;

    m_Window->close();
    delete m_Window;
}

void Game::Load()
{
    tinyxml2::XMLDocument doc;
    if (doc.LoadFile("Game.xml") == tinyxml2::XML_NO_ERROR)
    {
        tinyxml2::XMLElement* gameElement = doc.FirstChildElement("Game");
        assert(gameElement);

        {
            // Read settings
            tinyxml2::XMLElement* element = gameElement->FirstChildElement("Settings");
            assert(element);
            GlobalSettings::Init(*element);
        }
    }

    m_Hud->Load();
}

void Game::LoadNextLevel()
{
    m_ScoreManager->NextLevel();

    int const level = std::min(m_ScoreManager->GetLevel(), m_LevelMax);
    char filename[FILENAME_MAX];
    sprintf_s(filename, "Levels/Level%02d.xml", level);

    m_Level = std::make_shared<Level>(*m_ScoreManager);
    m_Level->Load(filename);
}

void Game::Run()
{
    sf::Clock frameTimer;

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

        if (m_Level->IsComplete())
        {
            LoadNextLevel();
        }

        float const dt = std::min(frameTimer.restart().asSeconds(), 1.0f / 60.0f);
        if (!m_ScoreManager->GetGameOver())
        {
            m_Level->Update(dt);
        }

        m_Window->clear();
        m_Level->Draw(*m_Window);
        m_Hud->Draw(*m_Window);
        m_Window->display();
    }
}
