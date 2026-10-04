#include "Game.h"

#include <algorithm>
#include <cmath>

#include "raylib.h"

namespace {
const float ROUND_LENGTH = 180.0f; // seconds from sunset to full dark

// The sky fades from the first color to the second over the round.
const Color SUNSET_SKY_COLOR = {72, 52, 104, 255}; // dusky purple
const Color NIGHT_SKY_COLOR = {12, 14, 36, 255};   // deep navy

const int HUD_FONT_SIZE = 20;
const Color HUD_TEXT_COLOR = {255, 245, 225, 180}; // soft, slightly see-through white

const Color CRITTER1_COLOR = {240, 170, 190, 255}; // soft pink
const Color CRITTER2_COLOR = {160, 220, 200, 255}; // soft mint

const int FIREFLY_COUNT = 12; // delivered fireflies are replaced to keep this many

// Fireflies spawn in the meadow (left side); the village is on the right.
const int MEADOW_LEFT = 40;
const int MEADOW_RIGHT = 600;
const int MEADOW_TOP = 40;
const int MEADOW_BOTTOM = 500;

const Vector2 LANTERN_POSITIONS[] = {
    {720.0f, 100.0f}, {860.0f, 140.0f}, {780.0f, 230.0f}, {900.0f, 290.0f},
    {700.0f, 370.0f}, {830.0f, 410.0f}, {910.0f, 480.0f},
};

// Each extra firefly in a load is worth one more point than the last
// (1, 3, 6, 10, ...), so one big careful delivery beats several small ones.
int pointsForDelivery(int fireflyCount)
{
    return fireflyCount * (fireflyCount + 1) / 2;
}
}

Game::Game()
    : critter1({320.0f, 270.0f}, CRITTER1_COLOR, {KEY_W, KEY_S, KEY_A, KEY_D}),
      critter2({640.0f, 270.0f}, CRITTER2_COLOR, {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT}),
      lightString(critter1, critter2, fireflies),
      timeLeft(ROUND_LENGTH),
      score(0),
      biggestDelivery(0)
{
    for (int i = 0; i < FIREFLY_COUNT; i++) {
        spawnFirefly();
    }
    for (Vector2 position : LANTERN_POSITIONS) {
        lanterns.push_back(Lantern(position));
    }
}

void Game::update(float dt)
{
    timeLeft = std::max(0.0f, timeLeft - dt);

    critter1.update(dt);
    critter2.update(dt);
    for (Firefly& firefly : fireflies) {
        firefly.update(dt);
    }
    // Last, so it sees where everything is now and has the final say on where
    // carried fireflies sit.
    lightString.update(dt);

    deliverToLanterns();

    // Top the meadow back up so there is always something to catch.
    while ((int)fireflies.size() < FIREFLY_COUNT) {
        spawnFirefly();
    }
}

void Game::draw()
{
    float progress = 1.0f - timeLeft / ROUND_LENGTH; // 0 at sunset, 1 at full dark
    ClearBackground(ColorLerp(SUNSET_SKY_COLOR, NIGHT_SKY_COLOR, progress));

    for (const Lantern& lantern : lanterns) {
        lantern.draw();
    }
    lightString.draw(); // drawn before the critters so they sit on top of its ends
    for (const Firefly& firefly : fireflies) {
        firefly.draw();
    }
    critter1.draw();
    critter2.draw();

    drawHud();
}

void Game::spawnFirefly()
{
    Vector2 spawn = {
        (float)GetRandomValue(MEADOW_LEFT, MEADOW_RIGHT),
        (float)GetRandomValue(MEADOW_TOP, MEADOW_BOTTOM),
    };
    fireflies.push_back(Firefly(spawn));
}

void Game::deliverToLanterns()
{
    for (Lantern& lantern : lanterns) {
        if (lantern.isLit() || lightString.getLoadSize() == 0) {
            continue;
        }
        if (lightString.touches(lantern.getPosition(), lantern.getRadius())) {
            int delivered = lightString.deliverLoad();
            lantern.light(delivered);
            score += pointsForDelivery(delivered);
            biggestDelivery = std::max(biggestDelivery, delivered);
        }
    }
}

void Game::drawHud() const
{
    DrawText(TextFormat("Score %d", score), 16, 12, HUD_FONT_SIZE, HUD_TEXT_COLOR);

    // Round up so the clock shows 0:00 only once time has truly run out.
    int secondsLeft = (int)std::ceil(timeLeft);
    const char* clock = TextFormat("%d:%02d", secondsLeft / 60, secondsLeft % 60);
    int clockWidth = MeasureText(clock, HUD_FONT_SIZE);
    DrawText(clock, (GetScreenWidth() - clockWidth) / 2, 12, HUD_FONT_SIZE, HUD_TEXT_COLOR);
}
