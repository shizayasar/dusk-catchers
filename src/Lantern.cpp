#include "Lantern.h"

#include <algorithm>

namespace {
const float RADIUS = 14.0f;              // touch area for the string and the fireflies' clear zone
const float BIG_RADIUS = 26.0f;
const float BIG_SCALE = 1.8f;            // the big lantern is drawn this much larger

// The lamp, at normal size, measured from `position` (the middle of the glass).
// The big lantern scales all of these up.
const float GLASS_WIDTH = 16.0f;
const float GLASS_HEIGHT = 18.0f;
const float FRAME_THICKNESS = 2.0f;
const float CAP_HEIGHT = 11.0f;          // the pointed roof above the glass
const float CAP_OVERHANG = 3.0f;         // how far the roof sticks out past the glass
const float KNOB_RADIUS = 2.0f;
const float BASE_HEIGHT = 3.0f;          // the ledge under the glass
const float POST_WIDTH = 4.0f;
const float POST_HEIGHT = 20.0f;
const float FOOT_WIDTH = 14.0f;
const Color METAL_COLOR = {70, 56, 52, 255};         // dark iron, warmed by the evening
const Color GLASS_UNLIT_COLOR = {48, 54, 82, 255};   // dark, cool glass

// Brightness grows with the delivery, up to this many fireflies.
const int BRIGHTEST_AT = 12;
const Color GLASS_DIM_COLOR = {170, 98, 44, 255};    // one firefly: a faint amber
const Color GLASS_BRIGHT_COLOR = {255, 214, 130, 255}; // a full lantern: bright gold

// The glow around a lit lantern.
const float GLOW_DIMMEST = 30.0f;        // radius from a one-firefly delivery
const float GLOW_BRIGHTEST = 150.0f;     // radius from a full lantern
const Color GLOW_COLOR = {255, 150, 60, 255}; // orange; alpha set below

const float BLOOM_TIME = 0.5f;           // seconds for the glow to open fully

const int NEEDED_FONT_SIZE = 20;
const Color NEEDED_TEXT_COLOR = {255, 220, 150, 220};
}

Lantern::Lantern(Vector2 position, int loadNeeded)
    : position(position), loadNeeded(loadNeeded), lit(false), fireflyCount(0), bloom(0.0f)
{
}

void Lantern::update(float dt)
{
    if (lit && bloom < 1.0f) {
        bloom = std::min(1.0f, bloom + dt / BLOOM_TIME);
    }
}

void Lantern::draw() const
{
    float s = isBig() ? BIG_SCALE : 1.0f;
    float x = position.x;
    float y = position.y;
    Rectangle glassArea = {x - GLASS_WIDTH * s / 2.0f, y - GLASS_HEIGHT * s / 2.0f,
                           GLASS_WIDTH * s, GLASS_HEIGHT * s};
    float glassBottom = glassArea.y + glassArea.height;

    // The post and its foot, below the lamp.
    DrawRectangleRec({x - POST_WIDTH * s / 2.0f, glassBottom, POST_WIDTH * s, POST_HEIGHT * s}, METAL_COLOR);
    DrawRectangleRec({x - FOOT_WIDTH * s / 2.0f, glassBottom + POST_HEIGHT * s - BASE_HEIGHT * s,
                      FOOT_WIDTH * s, BASE_HEIGHT * s}, METAL_COLOR);

    // Lit glass shines brighter the more fireflies lit it, and fades in as the
    // lantern blooms open.
    Color glass = GLASS_UNLIT_COLOR;
    if (lit) {
        Color litGlass = ColorLerp(GLASS_DIM_COLOR, GLASS_BRIGHT_COLOR, brightness());
        glass = ColorLerp(GLASS_UNLIT_COLOR, litGlass, bloom);
    }
    DrawRectangleRec(glassArea, glass);

    // The frame: an outline, a bar down the middle splitting it into two panes,
    // and a ledge underneath.
    DrawRectangleLinesEx(glassArea, FRAME_THICKNESS * s, METAL_COLOR);
    DrawLineEx({x, glassArea.y}, {x, glassBottom}, 1.5f * s, METAL_COLOR);
    float ledgeWidth = glassArea.width + 2.0f * s;
    DrawRectangleRec({x - ledgeWidth / 2.0f, glassBottom - 1.0f * s, ledgeWidth, BASE_HEIGHT * s}, METAL_COLOR);

    // The pointed roof and the knob on top.
    float roofHalf = glassArea.width / 2.0f + CAP_OVERHANG * s;
    Vector2 peak = {x, glassArea.y - CAP_HEIGHT * s};
    Vector2 roofLeft = {x - roofHalf, glassArea.y};
    Vector2 roofRight = {x + roofHalf, glassArea.y};
    DrawTriangle(peak, roofLeft, roofRight, METAL_COLOR); // corners listed counter-clockwise, as raylib needs
    DrawCircleV({x, peak.y - KNOB_RADIUS * s * 0.5f}, KNOB_RADIUS * s, METAL_COLOR);

    if (isBig() && !lit) {
        // Show the load it needs, so players know what to aim for.
        const char* needed = TextFormat("%d", loadNeeded);
        int textWidth = MeasureText(needed, NEEDED_FONT_SIZE);
        DrawText(needed, (int)x - textWidth / 2, (int)y - NEEDED_FONT_SIZE / 2, NEEDED_FONT_SIZE,
                 NEEDED_TEXT_COLOR);
    }
}

void Lantern::drawGlow() const
{
    if (!lit) {
        return;
    }
    float s = isBig() ? BIG_SCALE : 1.0f;
    float shine = brightness();

    // Ease out: open quickly, then settle gently into full size.
    float opened = 1.0f - (1.0f - bloom) * (1.0f - bloom);
    float glowRadius = (GLOW_DIMMEST + (GLOW_BRIGHTEST - GLOW_DIMMEST) * shine) * s * opened;
    // A dim lantern's glow is fainter as well as smaller.
    float strength = 0.4f + 0.6f * shine;

    // Gradients that fade to nothing at their edge read as soft light: a wide
    // faint one for the reach of the glow and a small strong one near the glass.
    DrawCircleGradient((int)position.x, (int)position.y, glowRadius,
                       Fade(GLOW_COLOR, 0.35f * strength), Fade(GLOW_COLOR, 0.0f));
    DrawCircleGradient((int)position.x, (int)position.y, glowRadius * 0.35f,
                       Fade(GLOW_COLOR, 0.35f * strength), Fade(GLOW_COLOR, 0.0f));
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

float Lantern::brightness() const
{
    return std::min(fireflyCount, BRIGHTEST_AT) / (float)BRIGHTEST_AT;
}
