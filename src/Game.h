#pragma once

#include "Critter.h"
#include "LightString.h"

// Owns everything in the game. main() calls update() then draw() once per frame.
// Fireflies and lanterns will become members of this class in later milestones.
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
    LightString lightString; // must come after the critters it refers to
};
