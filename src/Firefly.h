#pragma once

#include "raylib.h"

// A common firefly that drifts around the meadow in slow, wandering loops.
class Firefly {
public:
    explicit Firefly(Vector2 startPosition);

    void update(float dt);
    void draw() const;

    Vector2 getPosition() const { return position; }
    bool isClinging() const { return clinging; }
    // False while clinging, and for a moment after being shaken loose.
    bool canBeCaught() const { return !clinging && catchCooldown <= 0.0f; }

    // Attach to the string at fraction t along it (0 = first critter, 1 = second).
    void clingAt(float t);
    // Move to our spot on the string, wherever its ends are now.
    void followString(Vector2 stringStart, Vector2 stringEnd);
    // Fall off the string and go back to drifting.
    void letGo();

private:
    void pickNewTurnRate();

    Vector2 position;
    float heading;      // direction of travel, in radians
    float turnRate;     // radians per second; the sign decides left or right loops
    float turnTimer;    // seconds until we pick a new turn rate
    bool clinging;
    float stringT;      // where along the string we cling, from 0 to 1
    float catchCooldown; // seconds until we can be caught again after letting go
};
