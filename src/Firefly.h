#pragma once

#include "raylib.h"

// A common firefly that drifts around the meadow in slow, wandering loops.
class Firefly {
public:
    explicit Firefly(Vector2 startPosition);

    void update(float dt);
    void draw() const;

    Vector2 getPosition() const { return position; }

private:
    void pickNewTurnRate();

    Vector2 position;
    float heading;      // direction of travel, in radians
    float turnRate;     // radians per second; the sign decides left or right loops
    float turnTimer;    // seconds until we pick a new turn rate
};
