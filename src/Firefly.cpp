#include "Firefly.h"

#include <algorithm>
#include <cmath>

#include "LightString.h"
#include "raymath.h"

namespace {
const float DRIFT_SPEED = 35.0f;       // pixels per second
const float MIN_TURN_RATE = 0.6f;      // radians per second; lower means wider loops
const float MAX_TURN_RATE = 1.4f;
const float TURN_CHANGE_TIME = 3.0f;   // seconds between picking a new turn rate
const float CATCH_DISTANCE = 10.0f;    // how close to the string a body must be to cling
// A firefly that just fell off is still touching the string; this stops the
// string grabbing it straight back.
const float CATCH_COOLDOWN = 1.0f;

// Shy fireflies.
const float SHY_RADIUS = 90.0f;  // a critter closer than this scares them
const float DART_SPEED = 140.0f; // pixels per second while fleeing

// Pair fireflies.
const float PAIR_LINK_LENGTH = 24.0f; // pixels between the two bodies
const float PAIR_SPIN_SPEED = 0.8f;   // radians per second the link turns

const float RADIUS = 4.0f;
const float HALO_RADIUS = 10.0f;
const Color COMMON_CORE_COLOR = {255, 235, 140, 255}; // warm yellow
const Color COMMON_HALO_COLOR = {255, 210, 100, 50};  // same hue, mostly transparent
const Color SHY_CORE_COLOR = {210, 255, 200, 255};    // pale mint, to tell them apart
const Color SHY_HALO_COLOR = {170, 240, 180, 50};
const Color PAIR_CORE_COLOR = {255, 205, 160, 255};   // soft peach
const Color PAIR_HALO_COLOR = {255, 170, 120, 50};
const Color PAIR_LINK_COLOR = {255, 205, 160, 120};

// raylib only gives random integers, so build a float from one.
float randomFloat(float min, float max)
{
    return min + (max - min) * GetRandomValue(0, 10000) / 10000.0f;
}

// Half of the link between a pair's two bodies, pointing from the center to one body.
Vector2 halfLink(float angle)
{
    return {std::cos(angle) * PAIR_LINK_LENGTH / 2.0f, std::sin(angle) * PAIR_LINK_LENGTH / 2.0f};
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
      catchCooldown(0.0f),
      partnerPosition(startPosition),
      partnerStringT(0.0f),
      linkAngle(randomFloat(0.0f, 2.0f * PI))
{
    if (kind == Kind::Pair) {
        position = Vector2Subtract(startPosition, halfLink(linkAngle));
        partnerPosition = Vector2Add(startPosition, halfLink(linkAngle));
    }
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

    // A pair drifts as one unit around the middle of its link.
    Vector2 center = position;
    if (kind == Kind::Pair) {
        center = Vector2Lerp(position, partnerPosition, 0.5f);
    }

    switch (kind) {
    case Kind::Common:
        break;
    case Kind::Shy: {
        float distance1 = Vector2Distance(center, critter1Position);
        float distance2 = Vector2Distance(center, critter2Position);
        Vector2 nearest = distance1 < distance2 ? critter1Position : critter2Position;
        if (std::min(distance1, distance2) < SHY_RADIUS) {
            // Face straight away from the nearest critter and flee.
            Vector2 away = Vector2Subtract(center, nearest);
            heading = std::atan2(away.y, away.x);
            speed = DART_SPEED;
        }
        break;
    }
    case Kind::Pair:
        linkAngle += PAIR_SPIN_SPEED * dt;
        break;
    }

    Vector2 forward = {std::cos(heading), std::sin(heading)};
    center = Vector2Add(center, Vector2Scale(forward, speed * dt));
    center.x = Clamp(center.x, RADIUS, GetScreenWidth() - RADIUS);
    center.y = Clamp(center.y, RADIUS, GetScreenHeight() - RADIUS);

    if (kind == Kind::Pair) {
        position = Vector2Subtract(center, halfLink(linkAngle));
        partnerPosition = Vector2Add(center, halfLink(linkAngle));
    } else {
        position = center;
    }
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
    case Kind::Pair:
        core = PAIR_CORE_COLOR;
        halo = PAIR_HALO_COLOR;
        DrawLineEx(position, partnerPosition, 1.5f, PAIR_LINK_COLOR);
        DrawCircleV(partnerPosition, HALO_RADIUS, halo);
        DrawCircleV(partnerPosition, RADIUS, core);
        break;
    }
    DrawCircleV(position, HALO_RADIUS, halo);
    DrawCircleV(position, RADIUS, core);
}

int Firefly::getCount() const
{
    switch (kind) {
    case Kind::Pair:
        return 2;
    case Kind::Common:
    case Kind::Shy:
        break;
    }
    return 1;
}

void Firefly::tryToCling(const LightString& string)
{
    switch (kind) {
    case Kind::Common:
    case Kind::Shy:
        if (string.touches(position, CATCH_DISTANCE)) {
            clinging = true;
            stringT = string.fractionAlong(position);
        }
        break;
    case Kind::Pair:
        // Both bodies in the same frame: the string has to line up with the link.
        if (string.touches(position, CATCH_DISTANCE) &&
            string.touches(partnerPosition, CATCH_DISTANCE)) {
            clinging = true;
            stringT = string.fractionAlong(position);
            partnerStringT = string.fractionAlong(partnerPosition);
        }
        break;
    }
}

void Firefly::followString(Vector2 stringStart, Vector2 stringEnd)
{
    // Keeping the same fraction t means we slide apart as the string stretches
    // and swing along when it turns.
    position = Vector2Lerp(stringStart, stringEnd, stringT);
    if (kind == Kind::Pair) {
        partnerPosition = Vector2Lerp(stringStart, stringEnd, partnerStringT);
    }
}

void Firefly::letGo()
{
    clinging = false;
    catchCooldown = CATCH_COOLDOWN;
    if (kind == Kind::Pair) {
        // Carry on spinning from however the string left the link.
        Vector2 link = Vector2Subtract(partnerPosition, position);
        linkAngle = std::atan2(link.y, link.x);
    }
}

void Firefly::pickNewTurnRate()
{
    turnRate = randomFloat(MIN_TURN_RATE, MAX_TURN_RATE);
    if (GetRandomValue(0, 1) == 0) {
        turnRate = -turnRate;
    }
    turnTimer = TURN_CHANGE_TIME;
}
