#pragma once

#include "raylib.h"

// A firefly. Every kind drifts around the meadow in slow, wandering loops;
// some kinds add their own behavior on top (see Kind).
class Firefly {
public:
    enum class Kind {
        Common, // just drifts
        Shy,    // darts away when a critter gets close
    };

    Firefly(Kind kind, Vector2 startPosition);

    // Fireflies react to where the critters are, so those are passed in each frame.
    void update(float dt, Vector2 critter1Position, Vector2 critter2Position);
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

    Kind kind;
    Vector2 position;
    float heading;       // direction of travel, in radians
    float turnRate;      // radians per second; the sign decides left or right loops
    float turnTimer;     // seconds until we pick a new turn rate
    bool clinging;
    float stringT;       // where along the string we cling, from 0 to 1
    float catchCooldown; // seconds until we can be caught again after letting go
};
