#include "Critter.h"

#include "raymath.h"

namespace {
const float SPEED = 220.0f; // pixels per second
const float RADIUS = 18.0f;
}

Critter::Critter(Vector2 startPosition, Color color, ControlKeys keys)
    : position(startPosition), color(color), keys(keys), moving(false)
{
}

void Critter::update(float dt)
{
    Vector2 direction = {0.0f, 0.0f};
    if (IsKeyDown(keys.up))    direction.y -= 1.0f;
    if (IsKeyDown(keys.down))  direction.y += 1.0f;
    if (IsKeyDown(keys.left))  direction.x -= 1.0f;
    if (IsKeyDown(keys.right)) direction.x += 1.0f;

    // Opposite keys cancel out, so check the direction rather than the keys.
    moving = direction.x != 0.0f || direction.y != 0.0f;

    // Without this, diagonal movement would be about 41% faster than straight
    // movement, because (1, 1) is longer than (1, 0).
    direction = Vector2Normalize(direction);

    // Scaling by dt makes the speed the same in pixels per second at any frame rate.
    position = Vector2Add(position, Vector2Scale(direction, SPEED * dt));

    // Keep the whole circle on screen, not just its center.
    position.x = Clamp(position.x, RADIUS, GetScreenWidth() - RADIUS);
    position.y = Clamp(position.y, RADIUS, GetScreenHeight() - RADIUS);
}

void Critter::draw() const
{
    DrawCircleV(position, RADIUS, color);
}
