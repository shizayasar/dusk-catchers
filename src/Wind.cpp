#include "Wind.h"

#include <cmath>

#include "raymath.h"

namespace {
const float MIN_CALM_TIME = 4.0f;  // seconds between gusts, chosen at random each time
const float MAX_CALM_TIME = 7.0f;
const float GUST_LENGTH = 2.5f;    // seconds a gust lasts, rising and falling

// Sail strain: a string this long, side-on to a gust of strength 100, adds this
// much shake meter per second. Shorter strings and smaller angles add less.
const float SAIL_STRAIN = 1.0f;
const float SAIL_REFERENCE_LENGTH = 300.0f;
const float SAIL_REFERENCE_STRENGTH = 100.0f;

const int STREAK_COUNT = 40;
const float STREAK_LENGTH = 28.0f;
const float STREAK_CALM_SPEED = 60.0f;   // pixels per second, so the direction shows even when calm
const float STREAK_GUST_SPEED = 420.0f;  // extra speed at the height of a gust
const float STREAK_CALM_ALPHA = 0.06f;
const float STREAK_GUST_ALPHA = 0.35f;
const Color STREAK_COLOR = {220, 225, 255, 255}; // pale moonlit blue-white
}

Wind::Wind(float strength, float directionDegrees)
    : strength(strength),
      direction({std::cos(directionDegrees * DEG2RAD), std::sin(directionDegrees * DEG2RAD)}),
      calmTimer(0.0f),
      gustTime(-1.0f)
{
    if (strength <= 0.0f) {
        return; // calm evening: no gusts, no streaks
    }
    scheduleNextGust();
    for (int i = 0; i < STREAK_COUNT; i++) {
        streaks.push_back({(float)GetRandomValue(0, GetScreenWidth()),
                           (float)GetRandomValue(0, GetScreenHeight())});
    }
}

void Wind::update(float dt)
{
    if (strength <= 0.0f) {
        return;
    }

    if (gustTime >= 0.0f) {
        gustTime += dt;
        if (gustTime >= GUST_LENGTH) {
            gustTime = -1.0f;
            scheduleNextGust();
        }
    } else {
        calmTimer -= dt;
        if (calmTimer <= 0.0f) {
            gustTime = 0.0f;
        }
    }

    float speed = STREAK_CALM_SPEED + STREAK_GUST_SPEED * gustAmount();
    float width = (float)GetScreenWidth();
    float height = (float)GetScreenHeight();
    for (Vector2& streak : streaks) {
        streak = Vector2Add(streak, Vector2Scale(direction, speed * dt));
        // Off one side, back in on the other, so the streaks never run out.
        if (streak.x < -STREAK_LENGTH) streak.x += width + 2 * STREAK_LENGTH;
        if (streak.x > width + STREAK_LENGTH) streak.x -= width + 2 * STREAK_LENGTH;
        if (streak.y < -STREAK_LENGTH) streak.y += height + 2 * STREAK_LENGTH;
        if (streak.y > height + STREAK_LENGTH) streak.y -= height + 2 * STREAK_LENGTH;
    }
}

void Wind::draw() const
{
    float alpha = STREAK_CALM_ALPHA + (STREAK_GUST_ALPHA - STREAK_CALM_ALPHA) * gustAmount();
    Color color = Fade(STREAK_COLOR, alpha);
    for (Vector2 streak : streaks) {
        DrawLineEx(streak, Vector2Add(streak, Vector2Scale(direction, STREAK_LENGTH)), 1.5f, color);
    }
}

Vector2 Wind::getPush() const
{
    return Vector2Scale(direction, strength * gustAmount());
}

float Wind::strainOn(Vector2 stringStart, Vector2 stringEnd) const
{
    // The 2D cross product of the string with the wind direction is the
    // string's length times how side-on it is: 0 pointing into the wind, the
    // full length broadside. Like a sail, that's how much wind it catches.
    Vector2 string = Vector2Subtract(stringEnd, stringStart);
    float caught = std::fabs(string.x * direction.y - string.y * direction.x);
    return SAIL_STRAIN * (caught / SAIL_REFERENCE_LENGTH)
         * (strength / SAIL_REFERENCE_STRENGTH) * gustAmount();
}

float Wind::gustAmount() const
{
    if (gustTime < 0.0f) {
        return 0.0f;
    }
    // Half a sine wave: builds up, peaks halfway, and dies away smoothly.
    return std::sin(PI * gustTime / GUST_LENGTH);
}

void Wind::scheduleNextGust()
{
    float fraction = GetRandomValue(0, 1000) / 1000.0f;
    calmTimer = MIN_CALM_TIME + (MAX_CALM_TIME - MIN_CALM_TIME) * fraction;
}
