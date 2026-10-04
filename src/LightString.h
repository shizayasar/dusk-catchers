#pragma once

#include "Critter.h"
#include "raylib.h"

// The glowing line joining the two critters. It doesn't own the critters; it
// keeps references to them so it can read where they are each frame.
class LightString {
public:
    LightString(const Critter& startCritter, const Critter& endCritter);

    void update(float dt);
    void draw() const;

private:
    const Critter& startCritter;
    const Critter& endCritter;
    Vector2 start;
    Vector2 end;
};
