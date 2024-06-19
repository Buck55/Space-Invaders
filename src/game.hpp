#pragma once
#include "spaceShip.hpp"
#include "obstacle.hpp"
#include "alien.hpp"
#include "mysteryShip.hpp"

class Game
{
    public:
        Game();
        ~Game();

        void Draw();
        void Update();
        void HandleInput();
        
        std::vector<Alien> aliens;
        
        bool run;
        int lives;
        int score;
        int highScore;
        
        Music music;
    private:
        void DeleteInactiveLasers();
        std::vector<Obstacle> CreateObstacles();
        std::vector<Alien> CreateAliens();
        void MoveAliens();
        void MoveDownAliens(int distance);
        void AlienShootLaser();
        void CheckForCollisions();
        void GameOver();
        void Reset();
        void InitGame();
        void CheckForHighScore();
        void SaveHighscoreToFile(int highscore);
        int LoadHighscoreFromFile();
        
        Spaceship spaceship;

        std::vector<Obstacle> obstacles;

        int aliensDirection;
        std::vector<Laser> alienLasers;
        constexpr static float alienShootInterval = 0.35;
        float timeLastAlienFired;

        MysteryShip mysteryShip;
        float mysteryShipSpawnInterval;
        float timeLastSpawn;

        Sound explosionSound;
};