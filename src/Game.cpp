#include "Game.h"

#include "raylib.h"

namespace {
// Deep dusk blue. Later the sky will fade from purple to navy over the round.
const Color SKY_COLOR = {24, 28, 58, 255};

const Color CRITTER1_COLOR = {240, 170, 190, 255}; // soft pink
}

Game::Game()
    : critter1({320.0f, 270.0f}, CRITTER1_COLOR, {KEY_W, KEY_S, KEY_A, KEY_D})
{
}

void Game::update(float dt)
{
    critter1.update(dt);
}

void Game::draw()
{
    ClearBackground(SKY_COLOR);
    critter1.draw();
}
