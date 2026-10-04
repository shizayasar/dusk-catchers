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
    SetTargetFPS(TARGET_FPS);

    Game game;

    // The game loop: runs once per frame until the window is closed (or Esc is
    // pressed, which is raylib's default quit key until we add a pause menu).
    while (!WindowShouldClose()) {
        game.update(GetFrameTime());

        // raylib requires all drawing to happen between BeginDrawing and EndDrawing.
        BeginDrawing();
        game.draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
