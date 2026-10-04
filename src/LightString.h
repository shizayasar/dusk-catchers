#pragma once

#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "raylib.h"

// The glowing line joining the two critters. It catches any firefly it touches
// and carries it. It doesn't own the critters or the fireflies (Game does); it
// keeps references to them so it can read and move them each frame.
class LightString {
public:
    LightString(const Critter& startCritter, const Critter& endCritter,
                std::vector<Firefly>& fireflies);

    void update(float dt);
    void draw() const;

private:
    const Critter& startCritter;
    const Critter& endCritter;
    std::vector<Firefly>& fireflies;
    Vector2 start;
    Vector2 end;
};
