#include "Firefly.h"

#include <cmath>

#include "LightString.h"
#include "Obstacle.h"
#include "raymath.h"

namespace {
const float MIN_TURN_RATE = 0.6f;      // radians per second; lower means wider loops
const float MAX_TURN_RATE = 1.4f;
const float TURN_CHANGE_TIME = 3.0f;   // seconds between picking a new turn rate
const float CATCH_DISTANCE = 10.0f;    // how close to the string a body must be to cling
// A firefly that just fell off is still touching the string; this stops the
// string grabbing it straight back.
const float CATCH_COOLDOWN = 1.0f;

const float RADIUS = 4.0f;
const float HALO_RADIUS = 10.0f;
const float REACH = 6.0f; // how close to an obstacle a firefly's center may get
}

Firefly::Firefly(Vector2 startPosition)
    : position(startPosition),
      heading(randomFloat(0.0f, 2.0f * PI)),
      clinging(false),
      stringT(0.0f),
      catchCooldown(0.0f),
      turnRate(0.0f),
      turnTimer(0.0f)
{
    pickNewTurnRate();
}

void Firefly::update(float dt, Vector2 critter1Position, Vector2 critter2Position)
{
    (void)critter1Position; // plain drifting ignores the critters
    (void)critter2Position;
    if (clinging) {
        return; // the string carries us; see followString()
    }
    wander(dt);
    position = moveForward(position, DRIFT_SPEED, dt);
}

void Firefly::tryToCling(const LightString& string)
{
    if (touches(string, position)) {
        clinging = true;
        stringT = string.fractionAlong(position);
    }
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

void Firefly::keepClearOf(const Obstacle& obstacle)
{
    if (clinging) {
        return;
    }
    if (obstacle.isWater()) {
        // Fireflies can cross water, but the string can rarely reach one out
        // there, so any firefly over it heads straight for the nearest bank.
        if (obstacle.covers(getCenter())) {
            Vector2 out = obstacle.nearestWayOut(getCenter());
            heading = std::atan2(out.y, out.x);
        }
        return;
    }
    moveCenterTo(obstacle.pushOut(getCenter(), getReach()));
}

void Firefly::keepClearOf(Vector2 center, float radius)
{
    if (clinging) {
        return;
    }
    moveCenterTo(Obstacle::pushOutOfCircle(getCenter(), getReach(), center, radius));
}

float Firefly::getReach() const
{
    return REACH;
}

void Firefly::wander(float dt)
{
    catchCooldown -= dt;

    // Flying forward while turning steadily traces a circle; changing the turn
    // rate now and then makes the loops wander instead of repeating forever.
    turnTimer -= dt;
    if (turnTimer <= 0.0f) {
        pickNewTurnRate();
    }
    heading += turnRate * dt;
}

Vector2 Firefly::moveForward(Vector2 from, float speed, float dt) const
{
    Vector2 forward = {std::cos(heading), std::sin(heading)};
    Vector2 to = Vector2Add(from, Vector2Scale(forward, speed * dt));
    return keepOnScreen(to, EDGE_MARGIN);
}

Vector2 Firefly::keepOnScreen(Vector2 point, float margin)
{
    point.x = Clamp(point.x, margin, GetScreenWidth() - margin);
    point.y = Clamp(point.y, margin, GetScreenHeight() - margin);
    return point;
}

bool Firefly::touches(const LightString& string, Vector2 point)
{
    return string.touches(point, CATCH_DISTANCE);
}

void Firefly::drawBody(Vector2 at, Color core, Color halo)
{
    DrawCircleV(at, HALO_RADIUS, halo);
    DrawCircleV(at, RADIUS, core);
}

// raylib only gives random integers, so build a float from one.
float Firefly::randomFloat(float min, float max)
{
    return min + (max - min) * GetRandomValue(0, 10000) / 10000.0f;
}

void Firefly::pickNewTurnRate()
{
    turnRate = randomFloat(MIN_TURN_RATE, MAX_TURN_RATE);
    if (GetRandomValue(0, 1) == 0) {
        turnRate = -turnRate;
    }
    turnTimer = TURN_CHANGE_TIME;
}
