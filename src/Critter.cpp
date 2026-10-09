#include "Critter.h"

#include <cmath>

#include "raymath.h"

namespace {
const float SPEED = 220.0f; // pixels per second
const float RADIUS = 18.0f;

// How quickly `look` catches up with the direction of movement; higher is snappier.
const float LOOK_EASING = 10.0f;
const float STRETCH = 0.14f;       // how much longer the body gets along the movement
const float SQUASH = 0.08f;        // how much thinner it gets across it
const float EYE_SPACING = 6.0f;    // pixels from the center line to each eye
const float EYE_HEIGHT = -4.0f;    // eyes sit a little above center
const float EYE_RADIUS = 2.5f;
const float EYE_LOOK_DISTANCE = 4.0f; // how far the eyes shift toward `look`
const Color EYE_COLOR = {40, 30, 50, 255};
}

Critter::Critter(Vector2 startPosition, Color color, ControlKeys keys)
    : position(startPosition), color(color), keys(keys), moving(false), look({0.0f, 0.0f})
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

    // Move a fraction of the way toward the target each frame. Using exp()
    // makes that fraction depend on dt, so it eases at the same rate at any
    // frame rate, just like multiplying movement by dt.
    float catchUp = 1.0f - std::exp(-LOOK_EASING * dt);
    look = Vector2Lerp(look, direction, catchUp);

    // Scaling by dt makes the speed the same in pixels per second at any frame rate.
    push(Vector2Scale(direction, SPEED * dt));
}

void Critter::push(Vector2 offset)
{
    position = Vector2Add(position, offset);

    // Keep the whole circle on screen, not just its center.
    position.x = Clamp(position.x, RADIUS, GetScreenWidth() - RADIUS);
    position.y = Clamp(position.y, RADIUS, GetScreenHeight() - RADIUS);
}

void Critter::draw() const
{
    // Longer along the movement and thinner across it. raylib's ellipses only
    // stretch sideways or up and down, so a diagonal stays round.
    float horizontal = std::fabs(look.x);
    float vertical = std::fabs(look.y);
    float width = RADIUS * (1.0f + STRETCH * horizontal - SQUASH * vertical);
    float height = RADIUS * (1.0f + STRETCH * vertical - SQUASH * horizontal);
    DrawEllipse((int)position.x, (int)position.y, width, height, color);

    Vector2 glance = Vector2Scale(look, EYE_LOOK_DISTANCE);
    Vector2 leftEye = {position.x - EYE_SPACING + glance.x, position.y + EYE_HEIGHT + glance.y};
    Vector2 rightEye = {position.x + EYE_SPACING + glance.x, position.y + EYE_HEIGHT + glance.y};
    DrawCircleV(leftEye, EYE_RADIUS, EYE_COLOR);
    DrawCircleV(rightEye, EYE_RADIUS, EYE_COLOR);
}

float Critter::getRadius() const
{
    return RADIUS;
}
