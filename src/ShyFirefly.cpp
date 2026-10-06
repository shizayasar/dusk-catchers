#include "ShyFirefly.h"

#include <algorithm>
#include <cmath>

#include "raymath.h"

namespace {
const float SHY_RADIUS = 90.0f;  // a critter closer than this scares it
const float DART_SPEED = 140.0f; // pixels per second while fleeing
const Color CORE_COLOR = {210, 255, 200, 255}; // pale mint, to tell it apart
const Color HALO_COLOR = {170, 240, 180, 50};
}

ShyFirefly::ShyFirefly(Vector2 startPosition)
    : Firefly(startPosition)
{
}

void ShyFirefly::update(float dt, Vector2 critter1Position, Vector2 critter2Position)
{
    if (clinging) {
        return; // the string carries us; see followString()
    }
    wander(dt);

    float speed = DRIFT_SPEED;
    float distance1 = Vector2Distance(position, critter1Position);
    float distance2 = Vector2Distance(position, critter2Position);
    if (std::min(distance1, distance2) < SHY_RADIUS) {
        // Face straight away from the nearest critter and flee.
        Vector2 nearest = distance1 < distance2 ? critter1Position : critter2Position;
        Vector2 away = Vector2Subtract(position, nearest);
        heading = std::atan2(away.y, away.x);
        speed = DART_SPEED;
    }
    position = moveForward(position, speed, dt);
}

void ShyFirefly::draw() const
{
    drawBody(position, CORE_COLOR, HALO_COLOR);
}
