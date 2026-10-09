#pragma once

#include <vector>

#include "raylib.h"

// Gusts that come and go, pushing the critters along and straining the string
// like a sail. A strength of 0 means a calm evening: nothing happens.
class Wind {
public:
    Wind(float strength = 0.0f, float directionDegrees = 0.0f);

    void update(float dt);
    void draw() const;

    // How hard the wind is pushing right now, in pixels per second (zero between gusts).
    Vector2 getPush() const;
    // Shake meter per second this gust puts on a string between these two ends.
    float strainOn(Vector2 stringStart, Vector2 stringEnd) const;

private:
    // 0 when calm, rising to 1 at the height of a gust and back to 0.
    float gustAmount() const;
    void scheduleNextGust();

    float strength;     // pixels per second of push at the height of a gust
    Vector2 direction;  // unit vector the wind blows toward
    float calmTimer;    // seconds until the next gust starts
    float gustTime;     // seconds into the current gust, or < 0 if calm
    std::vector<Vector2> streaks; // where each drifting streak is
};
