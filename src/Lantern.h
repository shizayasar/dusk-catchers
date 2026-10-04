#pragma once

#include "raylib.h"

// A village lantern. It starts unlit; delivering fireflies lights it, and a
// bigger delivery makes it glow brighter. Once lit it stays lit.
class Lantern {
public:
    explicit Lantern(Vector2 position);

    void draw() const;

    void light(int fireflyCount);

    Vector2 getPosition() const { return position; }
    float getRadius() const;
    bool isLit() const { return lit; }

private:
    Vector2 position;
    bool lit;
    int fireflyCount; // how many fireflies lit it; decides how far the glow reaches
};
