#pragma once

#include <vector>

#include "raylib.h"

// Short-lived sparks, such as the burst when a lantern lights. Each spark flies
// outward, slows down, and fades away.
class Particles {
public:
    // Throw `count` sparks out from `at` in random directions.
    void burst(Vector2 at, int count);
    void clear() { sparks.clear(); }

    void update(float dt);
    // Meant to be drawn with additive blending, since sparks are light.
    void draw() const;

private:
    struct Spark {
        Vector2 position;
        Vector2 velocity; // pixels per second
        float life;       // seconds left
        float lifetime;   // seconds it started with, to work out how faded it is
        Color color;
    };

    std::vector<Spark> sparks;
};
