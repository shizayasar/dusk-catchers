#include "PairFirefly.h"

#include <cmath>

#include "LightString.h"
#include "raymath.h"

namespace {
const float LINK_LENGTH = 24.0f; // pixels between the two bodies
const float SPIN_SPEED = 0.8f;   // radians per second the link turns
const Color CORE_COLOR = {255, 205, 160, 255}; // soft peach
const Color HALO_COLOR = {255, 170, 120, 50};
const Color LINK_COLOR = {255, 205, 160, 120};
const float LINK_THICKNESS = 1.5f;
}

PairFirefly::PairFirefly(Vector2 startPosition)
    : Firefly(startPosition),
      partnerPosition(startPosition),
      partnerStringT(0.0f),
      linkAngle(randomFloat(0.0f, 2.0f * PI))
{
    placeBodiesAround(startPosition);
}

void PairFirefly::update(float dt, Vector2 critter1Position, Vector2 critter2Position)
{
    (void)critter1Position; // pairs ignore the critters
    (void)critter2Position;
    if (clinging) {
        return; // the string carries us; see followString()
    }
    wander(dt);
    linkAngle += SPIN_SPEED * dt;

    // The pair drifts as one unit around the middle of its link.
    Vector2 center = Vector2Lerp(position, partnerPosition, 0.5f);
    placeBodiesAround(moveForward(center, DRIFT_SPEED, dt));
}

void PairFirefly::draw() const
{
    DrawLineEx(position, partnerPosition, LINK_THICKNESS, LINK_COLOR);
    drawBody(position, CORE_COLOR, HALO_COLOR);
    drawBody(partnerPosition, CORE_COLOR, HALO_COLOR);
}

void PairFirefly::tryToCling(const LightString& string)
{
    // Both bodies in the same frame: the string has to line up with the link.
    if (touches(string, position) && touches(string, partnerPosition)) {
        clinging = true;
        stringT = string.fractionAlong(position);
        partnerStringT = string.fractionAlong(partnerPosition);
    }
}

void PairFirefly::followString(Vector2 stringStart, Vector2 stringEnd)
{
    Firefly::followString(stringStart, stringEnd); // moves the first body
    partnerPosition = Vector2Lerp(stringStart, stringEnd, partnerStringT);
}

void PairFirefly::letGo()
{
    Firefly::letGo();
    // Carry on spinning from however the string left the link.
    Vector2 link = Vector2Subtract(partnerPosition, position);
    linkAngle = std::atan2(link.y, link.x);
}

void PairFirefly::placeBodiesAround(Vector2 center)
{
    Vector2 halfLink = {std::cos(linkAngle) * LINK_LENGTH / 2.0f,
                        std::sin(linkAngle) * LINK_LENGTH / 2.0f};
    position = Vector2Subtract(center, halfLink);
    partnerPosition = Vector2Add(center, halfLink);
}
