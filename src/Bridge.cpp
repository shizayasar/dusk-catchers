#include "Bridge.h"

namespace {
const Color DECK_COLOR = {112, 86, 70, 255};   // warm wood, lighter than the fences
const Color PLANK_GAP_COLOR = {74, 56, 48, 255};
const Color RAIL_COLOR = {62, 46, 42, 255};
const float PLANK_WIDTH = 10.0f;               // pixels between plank lines
const float RAIL_THICKNESS = 3.0f;
}

Bridge::Bridge(Rectangle area)
    : area(area)
{
}

void Bridge::draw() const
{
    DrawRectangleRec(area, DECK_COLOR);
    // Planks run across the bridge, so their gaps are lines from rail to rail.
    for (float x = area.x + PLANK_WIDTH; x < area.x + area.width; x += PLANK_WIDTH) {
        DrawLineEx({x, area.y}, {x, area.y + area.height}, 1.0f, PLANK_GAP_COLOR);
    }
    DrawRectangleRec({area.x, area.y, area.width, RAIL_THICKNESS}, RAIL_COLOR);
    DrawRectangleRec({area.x, area.y + area.height - RAIL_THICKNESS, area.width, RAIL_THICKNESS},
                     RAIL_COLOR);
}
