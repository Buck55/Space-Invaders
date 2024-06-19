#include <raylib.h>
#include <string>
#include "game.hpp"

std::string FormatWithLeadingZeros(int number, int width)
{
    std::string numberText = std::to_string(number);
    int leadingZeros = width - numberText.length();
    return numberText = std::string(leadingZeros, '0') + numberText;
}

int main()
{
    int windowWidth = 750;
    int windowHeight = 700;
    int offset = 50;

    InitWindow(windowWidth + offset, windowHeight + 2 * offset, "Space Invaders");
    InitAudioDevice();


    Font font = LoadFontEx("Font/Alien-Encounters-Regular.ttf", 64, 0, 0);
    Texture2D spaceshipImage = LoadTexture("Graphics/spaceship1.png");

    SetTargetFPS(60);

    Game game;

    while(WindowShouldClose() == false)
    {
        UpdateMusicStream(game.music);
        game.HandleInput();
        game.Update();

        BeginDrawing();

            ClearBackground(BLACK);
            DrawRectangleRoundedLines({10, 10, 780, 780}, 0.18f, 20, 2, BLUE);
            DrawLineEx({25, 730}, {775, 730}, 3, BLUE);

            if(game.run)
            {
                DrawTextEx(font, "LEVEL 01", {570, 745}, 34, 2, BLUE);
                DrawTextEx(font, "PRESS A,D and SPACE BAR", {240, 750}, 20, 2, BLUE);
            }
            else
            {
                 if (game.aliens.empty())
                {
                    DrawTextEx(font, "VICTORY", {570, 745}, 34, 2, BLUE);
                    DrawTextEx(font, "PRESS ENTER",{300, 750}, 20, 2, BLUE);
                }
                else
                {
                    DrawTextEx(font, "GAME OVER", {570, 745}, 34, 2, RED);
                    DrawTextEx(font, "PRESS ENTER", {240, 750}, 20, 2, BLUE);
                }
            }

            float x = 50.0;
            for(int i = 0; i < game.lives; i++)
            {
                DrawTextureV(spaceshipImage, {x, 745}, WHITE);
                x += 50;
            }

            DrawTextEx(font, "SCORE", {50, 15}, 34, 2, BLUE);
            std::string scoreText = FormatWithLeadingZeros(game.score, 5);
            DrawTextEx(font, scoreText.c_str(), {50, 40}, 34, 3, BLUE);

            DrawTextEx(font, "HIGHSCORE", {555, 15}, 34, 2, BLUE);
            std::string highScoreText = FormatWithLeadingZeros(game.highScore, 5);
            DrawTextEx(font, highScoreText.c_str(), {630, 40}, 34, 3, BLUE);

            game.Draw();

        EndDrawing();
    }

    CloseWindow();
    CloseAudioDevice();
}