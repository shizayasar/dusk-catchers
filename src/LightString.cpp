#include "LightString.h"

#include <cmath>

#include "raymath.h"

namespace {
const float CORE_THICKNESS = 3.0f;
const float HALO_THICKNESS = 10.0f;
const Color CORE_COLOR = {255, 230, 150, 255}; // warm yellow
const Color HALO_COLOR = {255, 210, 110, 60};  // same hue, mostly transparent

// Shaking loose. The shake meter runs from 0 (calm) to 1 (a firefly falls off).
const float MAX_LENGTH = 360.0f;          // pixels; stretching past this strains the string
const float SAFE_PULL_SPEED = 250.0f;     // how fast the ends may move apart from each other
                                          // (pixels per second) before it counts as yanking
const float SHAKE_PER_PULL_SPEED = 1.0f / 250.0f; // meter per second, per px/s over the limit
const float SHAKE_PER_STRETCH = 1.0f / 40.0f;     // meter per second, per pixel over max length
const float CALM_DOWN_RATE = 0.6f;        // meter drained per second while not strained
const float WARNING_LEVEL = 0.35f;        // above this, the string flickers
const float RELIEF_AFTER_DROP = 0.25f;    // after a drop, the meter falls back this much
const float FLICKER_SPEED = 30.0f;        // how fast the warning flicker pulses
const float FLICKER_DIMMING = 0.7f;       // how dark the flicker gets at its deepest

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
                         std::vector<std::unique_ptr<Firefly>>& fireflies)
    : startCritter(startCritter),
      endCritter(endCritter),
      fireflies(fireflies),
      start(startCritter.getPosition()),
      end(endCritter.getPosition()),
      previousStart(start),
      previousEnd(end),
      shake(0.0f)
{
}

void LightString::reset()
{
    start = previousStart = startCritter.getPosition();
    end = previousEnd = endCritter.getPosition();
    shake = 0.0f;
}

void LightString::update(float dt)
{
    previousStart = start;
    previousEnd = end;
    start = startCritter.getPosition();
    end = endCritter.getPosition();

    updateShake(dt);

    for (std::unique_ptr<Firefly>& firefly : fireflies) {
        // Each kind decides for itself what counts as being caught.
        if (firefly->canBeCaught()) {
            firefly->tryToCling(*this);
        }
        if (firefly->isClinging()) {
            firefly->followString(start, end);
        }
    }
}

void LightString::updateShake(float dt)
{
    if (dt <= 0.0f) {
        return; // no time passed, so no speed to measure
    }

    // If both ends move the same way the string is simply being carried. The
    // difference between their velocities is how hard it's being pulled apart
    // or swung around.
    Vector2 startVelocity = Vector2Scale(Vector2Subtract(start, previousStart), 1.0f / dt);
    Vector2 endVelocity = Vector2Scale(Vector2Subtract(end, previousEnd), 1.0f / dt);
    float pullSpeed = Vector2Length(Vector2Subtract(startVelocity, endVelocity));
    float overStretch = Vector2Distance(start, end) - MAX_LENGTH;

    float strain = 0.0f;
    if (pullSpeed > SAFE_PULL_SPEED) {
        strain += (pullSpeed - SAFE_PULL_SPEED) * SHAKE_PER_PULL_SPEED;
    }
    if (overStretch > 0.0f) {
        strain += overStretch * SHAKE_PER_STRETCH;
    }

    if (strain > 0.0f) {
        shake += strain * dt;
    } else {
        shake -= CALM_DOWN_RATE * dt;
    }
    shake = Clamp(shake, 0.0f, 1.0f);

    dropLooseFirefly();
}

void LightString::dropLooseFirefly()
{
    // Any carried firefly whose grip the meter has reached may fall. Most have a
    // grip of 1 (a full meter); fragile kinds slip off sooner.
    std::vector<int> loose;
    for (int i = 0; i < (int)fireflies.size(); i++) {
        if (fireflies[i]->isClinging() && shake >= fireflies[i]->getGrip()) {
            loose.push_back(i);
        }
    }
    if (loose.empty()) {
        return;
    }
    int pick = loose[GetRandomValue(0, (int)loose.size() - 1)];
    fireflies[pick]->letGo();

    // Easing off only partway means continued rough handling drops fireflies
    // one by one, a moment apart, rather than all at once.
    shake -= RELIEF_AFTER_DROP;
}

int LightString::getLoadSize() const
{
    int count = 0;
    for (const std::unique_ptr<Firefly>& firefly : fireflies) {
        if (firefly->isClinging()) {
            count += firefly->getCount();
        }
    }
    return count;
}

bool LightString::touches(Vector2 center, float radius) const
{
    float t = closestFractionAlong(center, start, end);
    Vector2 closestPoint = Vector2Lerp(start, end, t);
    return Vector2Distance(center, closestPoint) <= radius;
}

float LightString::fractionAlong(Vector2 p) const
{
    return closestFractionAlong(p, start, end);
}

int LightString::deliverLoad()
{
    int delivered = 0;
    // Only step forward when we keep a firefly: erasing shifts the next one into slot i.
    for (size_t i = 0; i < fireflies.size();) {
        if (fireflies[i]->isClinging()) {
            delivered += fireflies[i]->getCount();
            // Erasing the unique_ptr also frees the firefly it owns.
            fireflies.erase(fireflies.begin() + i);
        } else {
            i++;
        }
    }
    return delivered;
}

void LightString::draw() const
{
    // Past the warning level the string pulses darker, more strongly the closer
    // the meter is to full, so players can feel a drop coming.
    float brightness = 1.0f;
    if (shake > WARNING_LEVEL) {
        float danger = (shake - WARNING_LEVEL) / (1.0f - WARNING_LEVEL);
        float pulse = 0.5f + 0.5f * std::sin((float)GetTime() * FLICKER_SPEED);
        brightness = 1.0f - FLICKER_DIMMING * danger * pulse;
    }

    // A wide faint line under a thin bright one reads as a soft glow.
    DrawLineEx(start, end, HALO_THICKNESS, Fade(HALO_COLOR, HALO_COLOR.a / 255.0f * brightness));
    DrawLineEx(start, end, CORE_THICKNESS, Fade(CORE_COLOR, brightness));
}
