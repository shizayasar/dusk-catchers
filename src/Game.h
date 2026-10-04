#pragma once

// Owns everything in the game. main() calls update() then draw() once per frame.
// For milestone 1 there is nothing to update yet; critters, fireflies and
// lanterns will become members of this class in later milestones.
class Game {
public:
    // dt is the time in seconds since the last frame. Multiplying movement by dt
    // keeps speeds the same no matter how fast the computer runs.
    void update(float dt);
    void draw();
};
