#pragma once

#include "raylib.h"

// A village lantern. It starts unlit; delivering fireflies lights it, and a
// bigger delivery makes it glow brighter. Once lit it stays lit. A big lantern
// only lights from a single delivery of at least loadNeeded fireflies.
class Lantern {
public:
    explicit Lantern(Vector2 position, int loadNeeded = 1);

    // Grows the glow after lighting, so the lantern blooms open.
    void update(float dt);
    // The solid lantern. Drawn normally.
    void draw() const;
    // Its light, if lit. Meant to be drawn with additive blending, so
    // overlapping glows add up like real light.
    void drawGlow() const;

    void light(int fireflyCount);

    Vector2 getPosition() const { return position; }
    float getRadius() const;
    bool isLit() const { return lit; }
    int getLoadNeeded() const { return loadNeeded; }

private:
    bool isBig() const { return loadNeeded > 1; }

    Vector2 position;
    int loadNeeded;   // smallest delivery that lights it
    bool lit;
    int fireflyCount; // how many fireflies lit it; decides how far the glow reaches
    float bloom;      // 0 just lit, rising to 1 when the glow is fully open
};
