#include "Firefly.h"

#include <algorithm>
#include <cmath>

#include "raymath.h"

namespace {
const float DRIFT_SPEED = 35.0f;       // pixels per second
const float MIN_TURN_RATE = 0.6f;      // radians per second; lower means wider loops
const float MAX_TURN_RATE = 1.4f;
const float TURN_CHANGE_TIME = 3.0f;   // seconds between picking a new turn rate
// A firefly that just fell off is still touching the string; this stops the
// string grabbing it straight back.
const float CATCH_COOLDOWN = 1.0f;

// Shy fireflies.
const float SHY_RADIUS = 90.0f;  // a critter closer than this scares them
const float DART_SPEED = 140.0f; // pixels per second while fleeing

const float RADIUS = 4.0f;
const float HALO_RADIUS = 10.0f;
const Color COMMON_CORE_COLOR = {255, 235, 140, 255}; // warm yellow
const Color COMMON_HALO_COLOR = {255, 210, 100, 50};  // same hue, mostly transparent
const Color SHY_CORE_COLOR = {210, 255, 200, 255};    // pale mint, to tell them apart
const Color SHY_HALO_COLOR = {170, 240, 180, 50};

// raylib only gives random integers, so build a float from one.
float randomFloat(float min, float max)
{
    return min + (max - min) * GetRandomValue(0, 10000) / 10000.0f;
}
}

Firefly::Firefly(Kind kind, Vector2 startPosition)
    : kind(kind),
      position(startPosition),
      heading(randomFloat(0.0f, 2.0f * PI)),
      turnRate(0.0f),
      turnTimer(0.0f),
      clinging(false),
      stringT(0.0f),
      catchCooldown(0.0f)
{
    pickNewTurnRate();
}

void Firefly::update(float dt, Vector2 critter1Position, Vector2 critter2Position)
{
    if (clinging) {
        return; // the string carries us; see followString()
    }

    catchCooldown -= dt;

    // Flying forward while turning steadily traces a circle; changing the turn
    // rate now and then makes the loops wander instead of repeating forever.
    turnTimer -= dt;
    if (turnTimer <= 0.0f) {
        pickNewTurnRate();
    }
    heading += turnRate * dt;
    float speed = DRIFT_SPEED;

    switch (kind) {
    case Kind::Common:
        break;
    case Kind::Shy: {
        float distance1 = Vector2Distance(position, critter1Position);
        float distance2 = Vector2Distance(position, critter2Position);
        Vector2 nearest = distance1 < distance2 ? critter1Position : critter2Position;
        if (std::min(distance1, distance2) < SHY_RADIUS) {
            // Face straight away from the nearest critter and flee.
            Vector2 away = Vector2Subtract(position, nearest);
            heading = std::atan2(away.y, away.x);
            speed = DART_SPEED;
        }
        break;
    }
    }

    Vector2 forward = {std::cos(heading), std::sin(heading)};
    position = Vector2Add(position, Vector2Scale(forward, speed * dt));

    position.x = Clamp(position.x, RADIUS, GetScreenWidth() - RADIUS);
    position.y = Clamp(position.y, RADIUS, GetScreenHeight() - RADIUS);
}

void Firefly::draw() const
{
    Color core = COMMON_CORE_COLOR;
    Color halo = COMMON_HALO_COLOR;
    switch (kind) {
    case Kind::Common:
        break;
    case Kind::Shy:
        core = SHY_CORE_COLOR;
        halo = SHY_HALO_COLOR;
        break;
    }
    DrawCircleV(position, HALO_RADIUS, halo);
    DrawCircleV(position, RADIUS, core);
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

void Firefly::letGo()
{
    clinging = false;
    catchCooldown = CATCH_COOLDOWN;
}

void Firefly::pickNewTurnRate()
{
    turnRate = randomFloat(MIN_TURN_RATE, MAX_TURN_RATE);
    if (GetRandomValue(0, 1) == 0) {
        turnRate = -turnRate;
    }
    turnTimer = TURN_CHANGE_TIME;
}
