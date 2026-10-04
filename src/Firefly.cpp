#include "Firefly.h"

#include <cmath>

#include "raymath.h"

namespace {
const float DRIFT_SPEED = 35.0f;       // pixels per second
const float MIN_TURN_RATE = 0.6f;      // radians per second; lower means wider loops
const float MAX_TURN_RATE = 1.4f;
const float TURN_CHANGE_TIME = 3.0f;   // seconds between picking a new turn rate
const float RADIUS = 4.0f;
const float HALO_RADIUS = 10.0f;
const Color CORE_COLOR = {255, 235, 140, 255}; // warm yellow
const Color HALO_COLOR = {255, 210, 100, 50};  // same hue, mostly transparent

// raylib only gives random integers, so build a float from one.
float randomFloat(float min, float max)
{
    return min + (max - min) * GetRandomValue(0, 10000) / 10000.0f;
}
}

Firefly::Firefly(Vector2 startPosition)
    : position(startPosition),
      heading(randomFloat(0.0f, 2.0f * PI)),
      turnRate(0.0f),
      turnTimer(0.0f),
      clinging(false),
      stringT(0.0f)
{
    pickNewTurnRate();
}

void Firefly::update(float dt)
{
    if (clinging) {
        return; // the string carries us; see followString()
    }

    // Flying forward while turning steadily traces a circle; changing the turn
    // rate now and then makes the loops wander instead of repeating forever.
    turnTimer -= dt;
    if (turnTimer <= 0.0f) {
        pickNewTurnRate();
    }
    heading += turnRate * dt;

    Vector2 forward = {std::cos(heading), std::sin(heading)};
    position = Vector2Add(position, Vector2Scale(forward, DRIFT_SPEED * dt));

    position.x = Clamp(position.x, RADIUS, GetScreenWidth() - RADIUS);
    position.y = Clamp(position.y, RADIUS, GetScreenHeight() - RADIUS);
}

void Firefly::draw() const
{
    DrawCircleV(position, HALO_RADIUS, HALO_COLOR);
    DrawCircleV(position, RADIUS, CORE_COLOR);
}

void Firefly::clingAt(float t)
{
    clinging = true;
    stringT = t;
}

void Firefly::followString(Vector2 stringStart, Vector2 stringEnd)
{
    // Keeping the same fraction t means we slide apart as the string stretches
    // and swing along when it turns.
    position = Vector2Lerp(stringStart, stringEnd, stringT);
}

void Firefly::pickNewTurnRate()
{
    turnRate = randomFloat(MIN_TURN_RATE, MAX_TURN_RATE);
    if (GetRandomValue(0, 1) == 0) {
        turnRate = -turnRate;
    }
    turnTimer = TURN_CHANGE_TIME;
}
