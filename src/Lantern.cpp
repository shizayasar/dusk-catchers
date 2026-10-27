#include "Lantern.h"

#include <algorithm>
#include <cmath>

namespace {
const float RADIUS = 14.0f;              // touch area for the string and the fireflies' clear zone
const float BIG_RADIUS = 26.0f;
const float BIG_SCALE = 1.8f;            // the big lantern's jar and glow are this much larger

// The jar, at normal size. The big lantern scales all of these up.
const float JAR_WIDTH = 22.0f;
const float JAR_HEIGHT = 24.0f;
const float JAR_ROUNDNESS = 0.35f;       // how rounded the jar's corners are (0 to 1)
const float LID_WIDTH = 17.0f;
const float LID_HEIGHT = 5.0f;
const float HANDLE_RADIUS = 7.0f;
const float HANDLE_THICKNESS = 1.5f;
const float SHINE_INSET = 4.0f;          // the shine line sits this far in from the left
const Color GLASS_UNLIT_COLOR = {48, 54, 82, 255};    // dark, cool glass
const Color GLASS_RIM_COLOR = {130, 135, 170, 255};
const Color SHINE_COLOR = {255, 255, 255, 45};
const Color LID_COLOR = {96, 74, 64, 255};            // tin, darkened by the evening
const Color HANDLE_COLOR = {140, 130, 120, 255};

// Brightness grows with the delivery, up to this many fireflies.
const int BRIGHTEST_AT = 12;
const Color GLASS_DIM_COLOR = {150, 92, 42, 255};     // one firefly: a faint amber
const Color GLASS_BRIGHT_COLOR = {255, 196, 96, 255};  // a full jar: rich gold, not white,
                                                      // so the fireflies inside still show

// The fireflies inside the jar.
const float INSIDE_DOT_RADIUS = 1.6f;
const float INSIDE_BOB = 1.5f;           // pixels they drift up and down
const float INSIDE_BOB_SPEED = 2.0f;     // radians per second
const Color INSIDE_DOT_COLOR = {255, 252, 225, 255};
const float INSIDE_HALO_RADIUS = 2.6f;  // a tiny glow, so each firefly stands out from the glass
const Color INSIDE_HALO_COLOR = {255, 240, 170, 110};

// The glow around a lit jar.
const float GLOW_DIMMEST = 30.0f;        // radius from a one-firefly delivery
const float GLOW_BRIGHTEST = 150.0f;     // radius from a full jar
const Color GLOW_COLOR = {255, 150, 60, 255}; // orange; alpha set per halo below

const float BLOOM_TIME = 0.5f;           // seconds for the glow to open fully

const int NEEDED_FONT_SIZE = 20;
const Color NEEDED_TEXT_COLOR = {255, 220, 150, 200};
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
    float scale = isBig() ? BIG_SCALE : 1.0f;
    float width = JAR_WIDTH * scale;
    float height = JAR_HEIGHT * scale;
    Rectangle jar = {position.x - width / 2.0f, position.y - height / 2.0f + 2.0f * scale, width, height};
    float lidTop = jar.y - LID_HEIGHT * scale + 1.0f;

    // The wire handle: the top half of a thin ring above the lid.
    DrawRing({position.x, lidTop}, (HANDLE_RADIUS - HANDLE_THICKNESS) * scale, HANDLE_RADIUS * scale,
             180.0f, 360.0f, 16, HANDLE_COLOR);

    // Lit glass shines brighter the more fireflies are inside, and fades in
    // as the lantern blooms open.
    Color glass = GLASS_UNLIT_COLOR;
    if (lit) {
        Color litGlass = ColorLerp(GLASS_DIM_COLOR, GLASS_BRIGHT_COLOR, brightness());
        glass = ColorLerp(GLASS_UNLIT_COLOR, litGlass, bloom);
    }
    DrawRectangleRounded(jar, JAR_ROUNDNESS, 8, glass);
    DrawRectangleRoundedLinesEx(jar, JAR_ROUNDNESS, 8, 1.0f, GLASS_RIM_COLOR);
    DrawLineEx({jar.x + SHINE_INSET * scale, jar.y + 5.0f * scale},
               {jar.x + SHINE_INSET * scale, jar.y + jar.height - 5.0f * scale}, 2.0f, SHINE_COLOR);

    DrawRectangleRec({position.x - LID_WIDTH * scale / 2.0f, lidTop, LID_WIDTH * scale, LID_HEIGHT * scale},
                     LID_COLOR);

    if (isBig() && !lit) {
        // Show the load it needs, so players know what to aim for.
        const char* needed = TextFormat("%d", loadNeeded);
        int textWidth = MeasureText(needed, NEEDED_FONT_SIZE);
        DrawText(needed, (int)position.x - textWidth / 2, (int)(jar.y + jar.height / 2.0f) - NEEDED_FONT_SIZE / 2,
                 NEEDED_FONT_SIZE, NEEDED_TEXT_COLOR);
    }
}

void Lantern::drawGlow() const
{
    if (!lit) {
        return;
    }
    float scale = isBig() ? BIG_SCALE : 1.0f;
    float shine = brightness();

    // Ease out: open quickly, then settle gently into full size.
    float opened = 1.0f - (1.0f - bloom) * (1.0f - bloom);
    float glowRadius = (GLOW_DIMMEST + (GLOW_BRIGHTEST - GLOW_DIMMEST) * shine) * scale * opened;
    // A dim lantern's glow is fainter as well as smaller.
    float strength = 0.4f + 0.6f * shine;

    // Gradients that fade to nothing at their edge read as soft light: a wide
    // faint one for the reach of the glow and a small strong one near the jar.
    DrawCircleGradient((int)position.x, (int)position.y, glowRadius,
                       Fade(GLOW_COLOR, 0.35f * strength), Fade(GLOW_COLOR, 0.0f));
    DrawCircleGradient((int)position.x, (int)position.y, glowRadius * 0.35f,
                       Fade(GLOW_COLOR, 0.35f * strength), Fade(GLOW_COLOR, 0.0f));

    // The delivered fireflies, drifting inside the jar. Stepping across and
    // down by these two fixed fractions (the "R2" sequence) scatters points
    // evenly in 2D without any randomness, so each keeps its place from frame
    // to frame and they never line up in rows.
    float width = JAR_WIDTH * scale - 8.0f;
    float height = JAR_HEIGHT * scale - 8.0f;
    float left = position.x - width / 2.0f;
    float top = position.y - height / 2.0f + 2.0f * scale;
    int dots = std::min(fireflyCount, BRIGHTEST_AT);
    for (int i = 0; i < dots; i++) {
        float across = std::fmod(0.5f + i * 0.7548777f, 1.0f);
        float down = std::fmod(0.5f + i * 0.5698403f, 1.0f);
        float bob = std::sin((float)GetTime() * INSIDE_BOB_SPEED + i) * INSIDE_BOB;
        Vector2 spot = {left + across * width, top + down * height + bob};
        DrawCircleV(spot, INSIDE_HALO_RADIUS * scale, Fade(INSIDE_HALO_COLOR, INSIDE_HALO_COLOR.a / 255.0f * bloom));
        DrawCircleV(spot, INSIDE_DOT_RADIUS * scale, Fade(INSIDE_DOT_COLOR, bloom));
    }
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
