#include "Game.h"

#include "raylib.h"

namespace {
// Deep dusk blue. Later the sky will fade from purple to navy over the round.
const Color SKY_COLOR = {24, 28, 58, 255};
}

void Game::update(float dt)
{
    (void)dt; // nothing moves yet; this silences the "unused parameter" warning
}

void Game::draw()
{
    ClearBackground(SKY_COLOR);
}
