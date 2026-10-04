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

const int RESULTS_TITLE_SIZE = 40;
const int RESULTS_LINE_SIZE = 24;
const Color RESULTS_TITLE_COLOR = {255, 220, 150, 255}; // warm lantern yellow
const Color RESULTS_TEXT_COLOR = {255, 245, 225, 230};
const Color RESULTS_HINT_COLOR = {255, 245, 225, 140};

const Vector2 CRITTER1_START = {320.0f, 270.0f};
const Vector2 CRITTER2_START = {640.0f, 270.0f};
const Color CRITTER1_COLOR = {240, 170, 190, 255}; // soft pink
const Color CRITTER2_COLOR = {160, 220, 200, 255}; // soft mint
const ControlKeys CRITTER1_KEYS = {KEY_W, KEY_S, KEY_A, KEY_D};
const ControlKeys CRITTER2_KEYS = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

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

void drawCenteredText(const char* text, int y, int fontSize, Color color)
{
    int width = MeasureText(text, fontSize);
    DrawText(text, (GetScreenWidth() - width) / 2, y, fontSize, color);
}
}

Game::Game()
    : critter1(CRITTER1_START, CRITTER1_COLOR, CRITTER1_KEYS),
      critter2(CRITTER2_START, CRITTER2_COLOR, CRITTER2_KEYS),
      lightString(critter1, critter2, fireflies),
      screen(Screen::Playing),
      timeLeft(ROUND_LENGTH),
      score(0),
      biggestDelivery(0)
{
    startRound();
}

void Game::update(float dt)
{
    switch (screen) {
    case Screen::Playing:
        updatePlaying(dt);
        break;
    case Screen::Results:
        if (IsKeyPressed(KEY_ENTER)) {
            startRound();
        }
        break;
    }
}

void Game::draw()
{
    switch (screen) {
    case Screen::Playing:
        drawPlaying();
        break;
    case Screen::Results:
        drawResults();
        break;
    }
}

void Game::startRound()
{
    // Replacing the critters with fresh ones puts them back at their start spots.
    // The string keeps working because it refers to these same two members.
    critter1 = Critter(CRITTER1_START, CRITTER1_COLOR, CRITTER1_KEYS);
    critter2 = Critter(CRITTER2_START, CRITTER2_COLOR, CRITTER2_KEYS);

    fireflies.clear();
    for (int i = 0; i < FIREFLY_COUNT; i++) {
        spawnFirefly();
    }

    lanterns.clear();
    for (Vector2 position : LANTERN_POSITIONS) {
        lanterns.push_back(Lantern(position));
    }

    timeLeft = ROUND_LENGTH;
    score = 0;
    biggestDelivery = 0;
    screen = Screen::Playing;
}

void Game::updatePlaying(float dt)
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

    if (timeLeft <= 0.0f) {
        screen = Screen::Results;
    }
}

void Game::drawPlaying() const
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

void Game::drawResults() const
{
    // Only the village under the night sky, so the lanterns you lit stand out.
    ClearBackground(NIGHT_SKY_COLOR);
    for (const Lantern& lantern : lanterns) {
        lantern.draw();
    }

    drawCenteredText("Night has fallen", 150, RESULTS_TITLE_SIZE, RESULTS_TITLE_COLOR);
    drawCenteredText(TextFormat("Lanterns lit: %d of %d", countLitLanterns(), (int)lanterns.size()),
                     230, RESULTS_LINE_SIZE, RESULTS_TEXT_COLOR);
    drawCenteredText(TextFormat("Score: %d", score), 270, RESULTS_LINE_SIZE, RESULTS_TEXT_COLOR);
    drawCenteredText(TextFormat("Biggest delivery: %d %s", biggestDelivery,
                                biggestDelivery == 1 ? "firefly" : "fireflies"),
                     310, RESULTS_LINE_SIZE, RESULTS_TEXT_COLOR);
    drawCenteredText("Press Enter to play again", 390, HUD_FONT_SIZE, RESULTS_HINT_COLOR);
}

void Game::drawHud() const
{
    DrawText(TextFormat("Score %d", score), 16, 12, HUD_FONT_SIZE, HUD_TEXT_COLOR);

    // Round up so the clock shows 0:00 only once time has truly run out.
    int secondsLeft = (int)std::ceil(timeLeft);
    drawCenteredText(TextFormat("%d:%02d", secondsLeft / 60, secondsLeft % 60),
                     12, HUD_FONT_SIZE, HUD_TEXT_COLOR);
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

int Game::countLitLanterns() const
{
    int count = 0;
    for (const Lantern& lantern : lanterns) {
        if (lantern.isLit()) {
            count++;
        }
    }
    return count;
}
