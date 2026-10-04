#pragma once

#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "LightString.h"

// Owns everything in the game. main() calls update() then draw() once per frame.
// Lanterns will become members of this class in a later milestone.
class Game {
public:
    Game();

    // dt is the time in seconds since the last frame. Multiplying movement by dt
    // keeps speeds the same no matter how fast the computer runs.
    void update(float dt);
    void draw();

private:
    Critter critter1;
    Critter critter2;
    std::vector<Firefly> fireflies;
    LightString lightString; // must come after the critters it refers to
};
