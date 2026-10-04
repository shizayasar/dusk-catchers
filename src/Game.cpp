#include "Game.h"

#include "raylib.h"

namespace {
// Deep dusk blue. Later the sky will fade from purple to navy over the round.
const Color SKY_COLOR = {24, 28, 58, 255};

const Color CRITTER1_COLOR = {240, 170, 190, 255}; // soft pink
const Color CRITTER2_COLOR = {160, 220, 200, 255}; // soft mint

const int FIREFLY_COUNT = 12;
const int SPAWN_MARGIN = 40; // keep new fireflies away from the screen edges
}

Game::Game()
    : critter1({320.0f, 270.0f}, CRITTER1_COLOR, {KEY_W, KEY_S, KEY_A, KEY_D}),
      critter2({640.0f, 270.0f}, CRITTER2_COLOR, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}),
      lightString(critter1, critter2)
{
    for (int i = 0; i < FIREFLY_COUNT; i++) {
        Vector2 spawn = {
            (float)GetRandomValue(SPAWN_MARGIN, GetScreenWidth() - SPAWN_MARGIN),
            (float)GetRandomValue(SPAWN_MARGIN, GetScreenHeight() - SPAWN_MARGIN),
        };
        fireflies.push_back(Firefly(spawn));
    }
}

void Game::update(float dt)
{
    critter1.update(dt);
    critter2.update(dt);
    for (Firefly& firefly : fireflies) {
        firefly.update(dt);
    }
    lightString.update(dt); // after the critters, so it reads where they are now
}

void Game::draw()
{
    ClearBackground(SKY_COLOR);
    lightString.draw(); // drawn first so the critters sit on top of its ends
    for (const Firefly& firefly : fireflies) {
        firefly.draw();
    }
    critter1.draw();
    critter2.draw();
}
