#pragma once

#include <memory>
#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "raylib.h"

// The glowing line joining the two critters. It catches any firefly it touches,
// carries it, shakes fireflies loose if handled roughly, and hands its whole
// load to a lantern. It doesn't own the critters or the fireflies (Game does);
// it keeps references to them so it can read and move them each frame.
class LightString {
public:
    LightString(const Critter& startCritter, const Critter& endCritter,
                std::vector<std::unique_ptr<Firefly>>& fireflies);

    // Forget past movement and calm the string, e.g. when a new round starts and
    // the critters jump back to their start spots.
    void reset();

    void update(float dt);
    void draw() const;

    // How many fireflies are clinging to the string right now (a pair counts as 2).
    int getLoadSize() const;
    // True if any part of the string is within radius of center.
    bool touches(Vector2 center, float radius) const;
    // How far along the string the point nearest p is, from 0 (first critter) to 1.
    float fractionAlong(Vector2 p) const;
    // Removes every carried firefly from the game and returns how many it counted as.
    int deliverLoad();

private:
    void updateShake(float dt);
    void dropLooseFirefly();

    const Critter& startCritter;
    const Critter& endCritter;
    std::vector<std::unique_ptr<Firefly>>& fireflies;
    Vector2 start;
    Vector2 end;
    Vector2 previousStart; // last frame's ends, used to measure how fast they move
    Vector2 previousEnd;
    float shake;           // 0 = calm, 1 = a firefly falls off
};
