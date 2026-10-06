#include "CommonFirefly.h"

namespace {
const Color CORE_COLOR = {255, 235, 140, 255}; // warm yellow
const Color HALO_COLOR = {255, 210, 100, 50};  // same hue, mostly transparent
}

CommonFirefly::CommonFirefly(Vector2 startPosition)
    : Firefly(startPosition)
{
}

void CommonFirefly::draw() const
{
    drawBody(position, CORE_COLOR, HALO_COLOR);
}
