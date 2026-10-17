#include "Game.h"

#include "raylib.h"

#include <memory>

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

namespace {
const int SCREEN_WIDTH = 960;
const int SCREEN_HEIGHT = 540;
const int TARGET_FPS = 60;

// Lives outside main() because in the browser, main() ends right away
// and the browser keeps calling updateDrawFrame() every frame.
std::unique_ptr<Game> game;

// One frame of the game: update, then draw.
void updateDrawFrame()
{
    game->update(GetFrameTime());

    BeginDrawing();
    game->draw();
    EndDrawing();
}
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Dusk Catchers");
    InitAudioDevice(); // before Game, which creates its sounds straight away
    SetExitKey(KEY_NULL); // Esc pauses the game instead of quitting

    game = std::make_unique<Game>();

#if defined(__EMSCRIPTEN__)
    // The browser calls updateDrawFrame() in step with the screen's refresh rate.
    emscripten_set_main_loop(updateDrawFrame, 0, 1);
#else
    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose()) {
        updateDrawFrame();
    }
#endif

    game.reset(); // free Game's sounds before the audio device closes
    CloseAudioDevice();
    CloseWindow();
    return 0;
}