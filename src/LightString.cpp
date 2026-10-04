#include "LightString.h"

#include "raymath.h"

namespace {
const float CORE_THICKNESS = 3.0f;
const float HALO_THICKNESS = 10.0f;
const Color CORE_COLOR = {255, 230, 150, 255}; // warm yellow
const Color HALO_COLOR = {255, 210, 110, 60};  // same hue, mostly transparent
const float CATCH_DISTANCE = 10.0f; // how close a firefly must be to the string to cling

// Returns how far along the segment from a to b the point closest to p lies,
// from 0 (at a) to 1 (at b). This is the heart of the line-vs-circle test.
float closestFractionAlong(Vector2 p, Vector2 a, Vector2 b)
{
    Vector2 ab = Vector2Subtract(b, a);
    float lengthSquared = Vector2DotProduct(ab, ab);
    if (lengthSquared == 0.0f) {
        return 0.0f; // critters on top of each other: the string is a single point
    }
    // Projecting p onto the line gives a fraction that can fall outside 0..1;
    // clamping keeps the closest point on the segment instead of the endless line.
    float t = Vector2DotProduct(Vector2Subtract(p, a), ab) / lengthSquared;
    return Clamp(t, 0.0f, 1.0f);
}
}

LightString::LightString(const Critter& startCritter, const Critter& endCritter,
                         std::vector<Firefly>& fireflies)
    : startCritter(startCritter),
      endCritter(endCritter),
      fireflies(fireflies),
      start(startCritter.getPosition()),
      end(endCritter.getPosition())
{
}

void LightString::update(float dt)
{
    (void)dt; // will be used to measure how fast the string moves (milestone 5)
    start = startCritter.getPosition();
    end = endCritter.getPosition();

    for (Firefly& firefly : fireflies) {
        if (!firefly.isClinging()) {
            float t = closestFractionAlong(firefly.getPosition(), start, end);
            Vector2 closestPoint = Vector2Lerp(start, end, t);
            if (Vector2Distance(firefly.getPosition(), closestPoint) <= CATCH_DISTANCE) {
                firefly.clingAt(t);
            }
        }
        if (firefly.isClinging()) {
            firefly.followString(start, end);
        }
    }
}

void LightString::draw() const
{
    // A wide faint line under a thin bright one reads as a soft glow.
    DrawLineEx(start, end, HALO_THICKNESS, HALO_COLOR);
    DrawLineEx(start, end, CORE_THICKNESS, CORE_COLOR);
}
