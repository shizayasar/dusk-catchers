#include "LightString.h"

namespace {
const float CORE_THICKNESS = 3.0f;
const float HALO_THICKNESS = 10.0f;
const Color CORE_COLOR = {255, 230, 150, 255}; // warm yellow
const Color HALO_COLOR = {255, 210, 110, 60};  // same hue, mostly transparent
}

LightString::LightString(const Critter& startCritter, const Critter& endCritter)
    : startCritter(startCritter),
      endCritter(endCritter),
      start(startCritter.getPosition()),
      end(endCritter.getPosition())
{
}

void LightString::update(float dt)
{
    (void)dt; // will be used to measure how fast the string moves (milestone 5)
    start = startCritter.getPosition();
    end = endCritter.getPosition();
}

void LightString::draw() const
{
    // A wide faint line under a thin bright one reads as a soft glow.
    DrawLineEx(start, end, HALO_THICKNESS, HALO_COLOR);
    DrawLineEx(start, end, CORE_THICKNESS, CORE_COLOR);
}
