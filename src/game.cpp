#include "game.hpp"
#include <fstream>
#include <iostream>

Game::Game()
{
    music = LoadMusicStream("Sounds/deep-space.mp3");
    explosionSound = LoadSound("Sounds/explosion.mp3");
    PlayMusicStream(music);
    InitGame();
}

Game::~Game()
{
    Alien::UnloadImages();
    UnloadMusicStream(music);
    UnloadSound(explosionSound);
}

void Game::Update()
{
    if(run)
    {
        double currentTime = GetTime();
        if(currentTime - timeLastSpawn > mysteryShipSpawnInterval)
        {
            mysteryShip.Spawn();
            timeLastSpawn = GetTime();
            mysteryShipSpawnInterval = GetRandomValue(10, 20);
        }

        for(auto& laser: spaceship.lasers)
        {
            laser.Update();
        }

        MoveAliens();

        AlienShootLaser();
        for(auto& laser: alienLasers)
        {
            laser.Update();
        }

        DeleteInactiveLasers();

        mysteryShip.Update();

        CheckForCollisions();

        if(aliens.empty())
        {
            GameOver();
        }
    }
    else
    {
       if(IsKeyDown(KEY_ENTER))
       {
           Reset();
           InitGame();
       }
    }
}

void Game::Draw()
{
    spaceship.Draw();

    for(auto& laser: spaceship.lasers)
    {
        laser.Draw();
    }

    for(auto& obstacle: obstacles)
    {
        obstacle.Draw();
    }

    for(auto& alien: aliens)
    {
        alien.Draw();
    }

    for(auto& laser: alienLasers)
    {
        laser.Draw();
    }

    mysteryShip.Draw();
}

void Game::HandleInput()
{
    if(run)
    {
        if(IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_A))
        {
            spaceship.MoveLeft();
        } 
        else if (IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_D))
        {
            spaceship.MoveRight();
        }
        else if (IsKeyDown(KEY_SPACE))
        {
            spaceship.FireLaser();
        }
    }
}

void Game::DeleteInactiveLasers()
{
    for(auto it = spaceship.lasers.begin(); it != spaceship.lasers.end();)
    {
        if(!it->active)
        {
            it = spaceship.lasers.erase(it);
        }
        else
        {
            ++it;
        }
    }

    for(auto it = alienLasers.begin(); it != alienLasers.end();)
    {
        if(!it->active)
        {
            it = alienLasers.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

std::vector<Obstacle> Game::CreateObstacles()
{
    int obstacleWidth = Obstacle::grid[0].size() * 3;
    float gap = (GetScreenWidth() - (4 * obstacleWidth)) / 5;

    for(int i = 0; i < 4; i++)
    {
        float offsetX = (i + 1) * gap + i * obstacleWidth;
        obstacles.push_back(Obstacle({offsetX, float(GetScreenHeight() - 200)}));
    }
    return obstacles;
}

std::vector<Alien> Game::CreateAliens()
{
    std::vector<Alien> aliens;
    for(int row = 0; row < 5; row++)
    {
        for(int column = 0; column < 11; column++)
        {
            int alienType;
            if(row == 0)
            {
                alienType = 3;
            }
            else if(row == 1 or row == 2)
            {
                alienType = 2;
            }
            else
            {
                alienType = 1;
            }

            float x = 75 + column * 55;
            float y = 110 + row * 55;
            aliens.push_back(Alien(alienType, {x, y}));
        }
    }
    return aliens;
}

void Game::MoveAliens()
{
    for(auto& alien: aliens)
    {
        if(alien.position.x + alien.alienImages[alien.type - 1].width > GetScreenWidth() - 25)
        {
            aliensDirection = -1;
            MoveDownAliens(4);
        }
        if(alien.position.x < 25)
        {
            aliensDirection = 1;
            MoveDownAliens(4);
        }

        alien.Update(aliensDirection);
    }
}

void Game::MoveDownAliens(int distance)
{
    for(auto& alien: aliens)
    {
        alien.position.y += distance;
    }
}

void Game::AlienShootLaser()
{
    double currentTime = GetTime();
    if(currentTime - timeLastAlienFired >= alienShootInterval and !aliens.empty())
    {
        int randomIndex = GetRandomValue(0, aliens.size() - 1);
        Alien& alien = aliens[randomIndex];
        alienLasers.push_back(Laser({alien.position.x + alien.alienImages[alien.type - 1].width / 2,
                                    alien.position.y + alien.alienImages[alien.type - 1].height / 2}, 6));
        timeLastAlienFired = GetTime();                           
    }
}

void Game::CheckForCollisions()
{
    //Spaceship Lasers
    for(auto& laser: spaceship.lasers)
    {
        auto it = aliens.begin();
        while(it != aliens.end())
        {
            if(CheckCollisionRecs(it->getRect(), laser.getRect()))
            {
                PlaySound(explosionSound);
                if(it->type == 1)
                {
                    score += 10; 
                }
                else if(it->type == 2)
                {
                    score += 20;
                }
                else if(it->type == 3)
                {
                    score += 30;
                }
                CheckForHighScore();

                it = aliens.erase(it);
                laser.active = false;
            }
            else
            {
                ++it;
            }
        }

        for(auto& obstacle: obstacles)
        {
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end())
            {
                if(CheckCollisionRecs(it->getRect(), laser.getRect()))
                {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                }
                else
                {
                    ++it;
                }
            }
        }

        if(CheckCollisionRecs(mysteryShip.getRect(), laser.getRect()))
        {
            mysteryShip.alive = false;
            laser.active = false;

            int possibleScores[] = {50, 100, 150, 300};
            int randomIndex = GetRandomValue(0, 3);
            score += possibleScores[randomIndex];

            CheckForHighScore();
            PlaySound(explosionSound);
        }
    }

    //Alien Lasers
    for(auto& laser: alienLasers)
    {
        if(CheckCollisionRecs(spaceship.getRect(), laser.getRect()))
        {
            laser.active = false;
            lives--;
            if(lives == 0)
            {
                GameOver();
            }
        }

        for(auto& obstacle: obstacles)
        {
            auto it = obstacle.blocks.begin();
            while(it != obstacle.blocks.end())
            {
                if(CheckCollisionRecs(it->getRect(), laser.getRect()))
                {
                    it = obstacle.blocks.erase(it);
                    laser.active = false;
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    //Alien Collision with Obstacles
    for(auto& alien: aliens)
    {
        for(auto& obstacle: obstacles)
        {
            auto it = obstacle.blocks.begin();
            while(it!= obstacle.blocks.end())
            {
                if(CheckCollisionRecs(it->getRect(), alien.getRect()))
                {
                    it = obstacle.blocks.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }

        if(CheckCollisionRecs(spaceship.getRect(), alien.getRect()))
        {
            GameOver();
        }
    }
}

void Game::GameOver()
{
    run = false;
}

void Game::InitGame()
{
    obstacles = CreateObstacles();

    aliens = CreateAliens();
    aliensDirection = 1;
    timeLastAlienFired = 0.0;

    timeLastSpawn = 0.0;
    mysteryShipSpawnInterval = GetRandomValue(10, 20);

    lives = 3;
    score = 0;
    highScore = LoadHighscoreFromFile();
    run = true;
}

void Game::CheckForHighScore()
{
    if(score > highScore)
    {
        highScore = score;
        SaveHighscoreToFile(highScore);
    }
}

void Game::SaveHighscoreToFile(int highscore)
{
    std::ofstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open())
    {
        highscoreFile << highscore;
        highscoreFile.close();
    }
    else
    {
        std::cerr << "Failed to save highscore to file" << std::endl;
    }
}

int Game::LoadHighscoreFromFile()
{
    int loadedHighscore = 0;
    std::ifstream highscoreFile("highscore.txt");
    if(highscoreFile.is_open())
    {
        highscoreFile >> loadedHighscore;
        highscoreFile.close();
    }
    else
    {
        std::cerr << "Failed to load highscore from file" << std::endl;
    }
    return loadedHighscore;
}

void Game::Reset()
{
    spaceship.Reset();
    aliens.clear();
    alienLasers.clear();
    obstacles.clear();
}
