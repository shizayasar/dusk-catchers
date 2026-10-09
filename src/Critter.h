#pragma once

#include "raylib.h"

// Which keys steer a critter. Giving each critter its own set lets one class
// serve both players (WASD for one, arrow keys for the other).
struct ControlKeys {
    KeyboardKey up;
    KeyboardKey down;
    KeyboardKey left;
    KeyboardKey right;
};

// One of the two players: a soft round creature that moves with its keys.
class Critter {
public:
    Critter(Vector2 startPosition, Color color, ControlKeys keys);

    void update(float dt);
    void draw() const;

    Vector2 getPosition() const { return position; }
    // True if the critter moved this frame.
    bool isMoving() const { return moving; }
    float getRadius() const;
    // Used when an obstacle pushes the critter back out of it.
    void setPosition(Vector2 newPosition) { position = newPosition; }

private:
    Vector2 position;
    Color color;
    ControlKeys keys;
    bool moving;
};
