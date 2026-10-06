#include "GoldenFirefly.h"

namespace {
const int COUNTS_AS = 3;     // fireflies' worth in a load
const float GRIP = 0.55f;    // falls off once the shake meter reaches this (normal is 1.0)

const Color CORE_COLOR = {255, 215, 90, 255};       // deep gold
const Color HALO_COLOR = {255, 190, 50, 70};
const Color OUTER_GLOW_COLOR = {255, 190, 50, 35};  // an extra wide halo, so it stands out
const float OUTER_GLOW_RADIUS = 20.0f;
}

GoldenFirefly::GoldenFirefly(Vector2 startPosition)
    : Firefly(startPosition)
{
}

void GoldenFirefly::draw() const
{
    DrawCircleV(position, OUTER_GLOW_RADIUS, OUTER_GLOW_COLOR);
    drawBody(position, CORE_COLOR, HALO_COLOR);
}

int GoldenFirefly::getCount() const
{
    return COUNTS_AS;
}

float GoldenFirefly::getGrip() const
{
    return GRIP;
}
