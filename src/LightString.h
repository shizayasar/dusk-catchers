#pragma once

#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "raylib.h"

// The glowing line joining the two critters. It catches any firefly it touches,
// carries it, and hands its whole load to a lantern. It doesn't own the critters or the fireflies (Game does); it
// keeps references to them so it can read and move them each frame.
class LightString {
public:
    LightString(const Critter& startCritter, const Critter& endCritter,
                std::vector<Firefly>& fireflies);

    void update(float dt);
    void draw() const;

    // How many fireflies are clinging to the string right now.
    int getLoadSize() const;
    // True if any part of the string is within radius of center.
    bool touches(Vector2 center, float radius) const;
    // Removes every carried firefly from the game and returns how many there were.
    int deliverLoad();

private:
    const Critter& startCritter;
    const Critter& endCritter;
    std::vector<Firefly>& fireflies;
    Vector2 start;
    Vector2 end;
};
