#include "Lantern.h"

#include <algorithm>

namespace {
const float RADIUS = 14.0f;              // the lantern body; also its touch area
const float BASE_GLOW_RADIUS = 40.0f;    // glow from a one-firefly delivery
const float GLOW_PER_FIREFLY = 12.0f;    // each extra firefly widens the glow this much
const int MAX_GLOW_FIREFLIES = 12;       // beyond this, the glow stops growing

// The big lantern.
const float BIG_RADIUS = 26.0f;
const float BIG_GLOW_SCALE = 1.8f;      // its glow reaches this much further
const int NEEDED_FONT_SIZE = 20;
const Color NEEDED_TEXT_COLOR = {255, 220, 150, 200};

const Color UNLIT_BODY_COLOR = {55, 50, 70, 255};
const Color UNLIT_RIM_COLOR = {120, 110, 140, 255};
const Color LIT_CORE_COLOR = {255, 200, 120, 255};  // warm orange-white
const Color LIT_GLOW_COLOR = {255, 150, 60, 255};   // orange; alpha set per halo below
}

Lantern::Lantern(Vector2 position, int loadNeeded)
    : position(position), loadNeeded(loadNeeded), lit(false), fireflyCount(0)
{
}

void Lantern::draw() const
{
    if (!lit) {
        DrawCircleV(position, getRadius(), UNLIT_BODY_COLOR);
        DrawCircleLinesV(position, getRadius(), UNLIT_RIM_COLOR);
        if (isBig()) {
            // Show the load it needs, so players know what to aim for.
            const char* needed = TextFormat("%d", loadNeeded);
            int width = MeasureText(needed, NEEDED_FONT_SIZE);
            DrawText(needed, (int)position.x - width / 2, (int)position.y - NEEDED_FONT_SIZE / 2,
                     NEEDED_FONT_SIZE, NEEDED_TEXT_COLOR);
        }
        return;
    }

    int glowFireflies = std::min(fireflyCount, MAX_GLOW_FIREFLIES);
    float glowRadius = BASE_GLOW_RADIUS + GLOW_PER_FIREFLY * (glowFireflies - 1);
    if (isBig()) {
        glowRadius *= BIG_GLOW_SCALE;
    }

    // Stacked transparent circles, faintest and widest first, read as a soft bloom.
    DrawCircleV(position, glowRadius, Fade(LIT_GLOW_COLOR, 0.08f));
    DrawCircleV(position, glowRadius * 0.6f, Fade(LIT_GLOW_COLOR, 0.12f));
    DrawCircleV(position, glowRadius * 0.35f, Fade(LIT_GLOW_COLOR, 0.2f));
    DrawCircleV(position, getRadius(), LIT_CORE_COLOR);
}

void Lantern::light(int count)
{
    lit = true;
    fireflyCount = count;
}

float Lantern::getRadius() const
{
    return isBig() ? BIG_RADIUS : RADIUS;
}
