#include "Obstacle.h"

#include <algorithm>

#include "raymath.h"

namespace {
const Color TREE_COLOR = {30, 52, 58, 255};      // dark blue-green canopy
const Color TREE_INNER_COLOR = {42, 68, 72, 255}; // a lighter middle, for some depth
const float TREE_INNER_SCALE = 0.6f;
const Color FENCE_COLOR = {92, 70, 62, 255};      // weathered wood
const Color FENCE_EDGE_COLOR = {62, 46, 42, 255};
}

Obstacle Obstacle::tree(Vector2 center, float radius)
{
    return Obstacle(Kind::Tree, center, radius, {0.0f, 0.0f, 0.0f, 0.0f});
}

Obstacle Obstacle::fence(Rectangle area)
{
    return Obstacle(Kind::Fence, {0.0f, 0.0f}, 0.0f, area);
}

Obstacle::Obstacle(Kind kind, Vector2 center, float radius, Rectangle area)
    : kind(kind), center(center), radius(radius), area(area)
{
}

void Obstacle::draw() const
{
    switch (kind) {
    case Kind::Tree:
        DrawCircleV(center, radius, TREE_COLOR);
        DrawCircleV(center, radius * TREE_INNER_SCALE, TREE_INNER_COLOR);
        break;
    case Kind::Fence:
        DrawRectangleRec(area, FENCE_COLOR);
        DrawRectangleLinesEx(area, 2.0f, FENCE_EDGE_COLOR);
        break;
    }
}

Vector2 Obstacle::pushOut(Vector2 point, float pointRadius) const
{
    switch (kind) {
    case Kind::Tree: {
        Vector2 away = Vector2Subtract(point, center);
        float distance = Vector2Length(away);
        float minimum = radius + pointRadius;
        if (distance >= minimum) {
            return point;
        }
        // Dead center has no "away" direction, so pick one.
        Vector2 direction = distance > 0.0f ? Vector2Scale(away, 1.0f / distance) : Vector2{0.0f, -1.0f};
        return Vector2Add(center, Vector2Scale(direction, minimum));
    }
    case Kind::Fence: {
        // The closest point on the rectangle to the circle's center.
        Vector2 closest = {Clamp(point.x, area.x, area.x + area.width),
                           Clamp(point.y, area.y, area.y + area.height)};
        Vector2 away = Vector2Subtract(point, closest);
        float distance = Vector2Length(away);
        if (distance >= pointRadius) {
            return point;
        }
        if (distance > 0.0f) {
            return Vector2Add(closest, Vector2Scale(away, pointRadius / distance));
        }
        // The center is inside the rectangle: leave through the nearest side.
        float toLeft = point.x - area.x;
        float toRight = area.x + area.width - point.x;
        float toTop = point.y - area.y;
        float toBottom = area.y + area.height - point.y;
        float nearest = std::min({toLeft, toRight, toTop, toBottom});
        if (nearest == toLeft) {
            point.x = area.x - pointRadius;
        } else if (nearest == toRight) {
            point.x = area.x + area.width + pointRadius;
        } else if (nearest == toTop) {
            point.y = area.y - pointRadius;
        } else {
            point.y = area.y + area.height + pointRadius;
        }
        return point;
    }
    }
    return point;
}

bool Obstacle::snags(Vector2 stringStart, Vector2 stringEnd) const
{
    switch (kind) {
    case Kind::Tree:
        // raylib clamps to the segment, the same distance test the string uses.
        return CheckCollisionCircleLine(center, radius, stringStart, stringEnd);
    case Kind::Fence: {
        if (CheckCollisionPointRec(stringStart, area) || CheckCollisionPointRec(stringEnd, area)) {
            return true;
        }
        // Otherwise the string can only touch the rectangle by crossing an edge.
        Vector2 corners[4] = {
            {area.x, area.y},
            {area.x + area.width, area.y},
            {area.x + area.width, area.y + area.height},
            {area.x, area.y + area.height},
        };
        for (int i = 0; i < 4; i++) {
            Vector2 crossing;
            if (CheckCollisionLines(stringStart, stringEnd, corners[i], corners[(i + 1) % 4], &crossing)) {
                return true;
            }
        }
        return false;
    }
    }
    return false;
}
