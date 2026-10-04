#pragma once

#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "Lantern.h"
#include "LightString.h"

// Owns everything in the game and keeps score. main() calls update() then
// draw() once per frame.
class Game {
public:
    Game();

    // dt is the time in seconds since the last frame. Multiplying movement by dt
    // keeps speeds the same no matter how fast the computer runs.
    void update(float dt);
    void draw();

private:
    void spawnFirefly();
    void deliverToLanterns();

    Critter critter1;
    Critter critter2;
    std::vector<Firefly> fireflies;
    LightString lightString; // must come after the critters and fireflies it refers to
    std::vector<Lantern> lanterns;

    int score;
    int biggestDelivery;
};
