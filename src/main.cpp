#include "Game.h"

#include "raylib.h"

namespace {
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;
const int TARGET_FPS = 60;
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dusk Catchers");
    InitAudioDevice(); // before Game, which creates its sounds straight away
    SetTargetFPS(TARGET_FPS);

    // The extra braces end Game's lifetime here, so its sounds are freed
    // before the audio device closes below.
    {
        Game game;

        // The game loop: runs once per frame until the window is closed (or Esc
        // is pressed, which is raylib's default quit key until we add a pause menu).
        while (!WindowShouldClose()) {
            game.update(GetFrameTime());

            // raylib requires all drawing to happen between BeginDrawing and EndDrawing.
            BeginDrawing();
            game.draw();
            EndDrawing();
        }
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
