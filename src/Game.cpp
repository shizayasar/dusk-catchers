#include "Game.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <utility>

#include "CommonFirefly.h"
#include "GoldenFirefly.h"
#include "PairFirefly.h"
#include "ShyFirefly.h"
#include "raylib.h"
#include "raymath.h"

namespace {
// Every .txt file here is an evening, played in filename order.
const char* const EVENINGS_FOLDER = "evenings";

// The sky fades from the first color to the second over the round.
const Color SUNSET_SKY_COLOR = {72, 52, 104, 255}; // dusky purple
const Color NIGHT_SKY_COLOR = {12, 14, 36, 255};   // deep navy

const int HUD_FONT_SIZE = 20;
const Color HUD_TEXT_COLOR = {255, 245, 225, 180}; // soft, slightly see-through white

const int TITLE_SIZE = 40;
const int LINE_SIZE = 24;
const Color TITLE_COLOR = {255, 220, 150, 255}; // warm lantern yellow
const Color TEXT_COLOR = {255, 245, 225, 230};
const Color HINT_COLOR = {255, 245, 225, 140};

// Evening select list.
const int LIST_TOP = 120;         // y of the first row
const int LIST_ROW_HEIGHT = 44;
const int LIST_LEFT = 250;        // x of the evening names
const int LIST_STARS_X = 640;     // x of the first star in each row
const Rectangle HIGHLIGHT_SIZE = {220.0f, 0.0f, 520.0f, 38.0f}; // x and size of the selected row's box
const Color HIGHLIGHT_COLOR = {255, 220, 150, 40};

// Stars.
const float STAR_OUTER_RADIUS = 12.0f;
const float STAR_INNER_RADIUS = 5.0f;
const float STAR_SPACING = 30.0f;
const Color STAR_EARNED_COLOR = {255, 210, 90, 255};  // gold
const Color STAR_EMPTY_COLOR = {255, 245, 225, 50};   // faint outline-ish white

// Each critter's keys are shown under it at the start of a round, then fade out.
const float CONTROLS_HINT_SHOW_TIME = 5.0f; // seconds fully visible
const float CONTROLS_HINT_FADE_TIME = 1.5f; // seconds to fade away after that
const float CONTROLS_HINT_OFFSET = 28.0f;   // pixels below the critter's center

const Color CRITTER1_COLOR = {240, 170, 190, 255}; // soft pink
const Color CRITTER2_COLOR = {160, 220, 200, 255}; // soft mint
const ControlKeys CRITTER1_KEYS = {KEY_W, KEY_S, KEY_A, KEY_D};
const ControlKeys CRITTER2_KEYS = {KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT};

// Shake meter per second while the string crosses a tree or fence.
const float SNAG_STRAIN = 1.2f;

// A golden firefly appears beside the middle of the string once both critters
// have stood still this long, if there isn't one out already.
const float GOLDEN_STILL_TIME = 2.0f;
const float GOLDEN_SPAWN_OFFSET = 50.0f; // pixels to the side of the string

// Each extra firefly in a load is worth one more point than the last
// (1, 3, 6, 10, ...), so one big careful delivery beats several small ones.
int pointsForDelivery(int fireflyCount)
{
    return fireflyCount * (fireflyCount + 1) / 2;
}

void drawTextBelow(const char* text, Vector2 position, Color color)
{
    int width = MeasureText(text, HUD_FONT_SIZE);
    DrawText(text, (int)position.x - width / 2, (int)(position.y + CONTROLS_HINT_OFFSET),
             HUD_FONT_SIZE, color);
}

void drawCenteredText(const char* text, int y, int fontSize, Color color)
{
    int width = MeasureText(text, fontSize);
    DrawText(text, (GetScreenWidth() - width) / 2, y, fontSize, color);
}

// raylib only fills a triangle whose corners go round in one particular
// direction, so swap two corners if they go the other way.
void drawTriangleAnyOrder(Vector2 a, Vector2 b, Vector2 c, Color color)
{
    float turn = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    if (turn > 0.0f) {
        std::swap(b, c);
    }
    DrawTriangle(a, b, c, color);
}

// A five-pointed star: ten points alternating between the outer and inner
// radius, each pair joined to the center by a triangle.
void drawStar(Vector2 center, Color color)
{
    const int POINTS = 10;
    Vector2 corners[POINTS];
    for (int i = 0; i < POINTS; i++) {
        float radius = (i % 2 == 0) ? STAR_OUTER_RADIUS : STAR_INNER_RADIUS;
        float angle = -PI / 2.0f + i * PI / 5.0f; // start pointing straight up
        corners[i] = {center.x + std::cos(angle) * radius, center.y + std::sin(angle) * radius};
    }
    for (int i = 0; i < POINTS; i++) {
        drawTriangleAnyOrder(center, corners[i], corners[(i + 1) % POINTS], color);
    }
}

// Three stars in a row starting at `left`, the first `earned` of them gold.
void drawStarRow(Vector2 left, int earned)
{
    for (int i = 0; i < 3; i++) {
        Vector2 center = {left.x + i * STAR_SPACING, left.y};
        drawStar(center, i < earned ? STAR_EARNED_COLOR : STAR_EMPTY_COLOR);
    }
}
}

Game::Game()
    : selectedEvening(0),
      critter1(evening.critter1Start, CRITTER1_COLOR, CRITTER1_KEYS),
      critter2(evening.critter2Start, CRITTER2_COLOR, CRITTER2_KEYS),
      lightString(critter1, critter2, fireflies),
      screen(Screen::EveningSelect),
      timeLeft(0.0f),
      score(0),
      biggestDelivery(0),
      stillTimer(0.0f)
{
    loadEvenings();
}

void Game::loadEvenings()
{
    // Sorting by filename is what lets "01-", "02-", ... set the order.
    FilePathList files = LoadDirectoryFiles(EVENINGS_FOLDER);
    std::vector<std::string> paths;
    for (unsigned int i = 0; i < files.count; i++) {
        if (IsFileExtension(files.paths[i], ".txt")) {
            paths.push_back(files.paths[i]);
        }
    }
    UnloadDirectoryFiles(files); // raylib allocated the list; hand it back
    std::sort(paths.begin(), paths.end());

    for (const std::string& path : paths) {
        Evening loaded;
        if (loaded.loadFromFile(path)) {
            evenings.push_back(loaded);
        }
    }
    bestStars.assign(evenings.size(), 0);

    if (evenings.empty()) {
        TraceLog(LOG_ERROR, "GAME: No evenings found in \"%s\"; run the game from the project folder",
                 EVENINGS_FOLDER);
    }
}

void Game::update(float dt)
{
    switch (screen) {
    case Screen::EveningSelect:
        updateEveningSelect();
        break;
    case Screen::Playing:
        updatePlaying(dt);
        break;
    case Screen::Results:
        if (IsKeyPressed(KEY_ENTER)) {
            screen = Screen::EveningSelect;
        }
        break;
    }
}

void Game::draw()
{
    switch (screen) {
    case Screen::EveningSelect:
        drawEveningSelect();
        break;
    case Screen::Playing:
        drawPlaying();
        break;
    case Screen::Results:
        drawResults();
        break;
    }
}

void Game::updateEveningSelect()
{
    if (evenings.empty()) {
        return;
    }
    int count = (int)evenings.size();
    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) {
        selectedEvening = (selectedEvening + count - 1) % count; // wrap from top to bottom
    }
    if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
        selectedEvening = (selectedEvening + 1) % count;
    }
    if (IsKeyPressed(KEY_ENTER)) {
        startRound();
    }
}

void Game::startRound()
{
    evening = evenings[selectedEvening];

    // Replacing the critters with fresh ones puts them back at their start spots.
    // The string keeps working because it refers to these same two members.
    critter1 = Critter(evening.critter1Start, CRITTER1_COLOR, CRITTER1_KEYS);
    critter2 = Critter(evening.critter2Start, CRITTER2_COLOR, CRITTER2_KEYS);
    // The critters just jumped to their start spots; don't let the string
    // mistake that for a violent yank.
    lightString.reset();

    fireflies.clear();
    for (int i = 0; i < evening.fireflyCount; i++) {
        spawnFirefly();
    }

    lanterns.clear();
    for (Vector2 position : evening.lanterns) {
        lanterns.push_back(Lantern(position));
    }

    wind = Wind(evening.windStrength, evening.windDirection);

    timeLeft = evening.length;
    score = 0;
    biggestDelivery = 0;
    stillTimer = 0.0f;
    screen = Screen::Playing;
}

void Game::finishRound()
{
    bestStars[selectedEvening] = std::max(bestStars[selectedEvening], starsEarned());
    screen = Screen::Results;
}

void Game::updatePlaying(float dt)
{
    timeLeft = std::max(0.0f, timeLeft - dt);

    wind.update(dt);

    critter1.update(dt);
    critter2.update(dt);
    Vector2 windPush = Vector2Scale(wind.getPush(), dt);
    critter1.push(windPush);
    critter2.push(windPush);
    lightString.addStrain(wind.strainOn(critter1.getPosition(), critter2.getPosition()));

    for (const Obstacle& obstacle : evening.obstacles) {
        critter1.setPosition(obstacle.pushOut(critter1.getPosition(), critter1.getRadius()));
        critter2.setPosition(obstacle.pushOut(critter2.getPosition(), critter2.getRadius()));
        if (obstacle.snags(critter1.getPosition(), critter2.getPosition())) {
            lightString.addStrain(SNAG_STRAIN);
        }
    }

    for (std::unique_ptr<Firefly>& firefly : fireflies) {
        firefly->update(dt, critter1.getPosition(), critter2.getPosition());
    }
    // Last, so it sees where everything is now and has the final say on where
    // carried fireflies sit.
    lightString.update(dt);

    deliverToLanterns();
    if (evening.goldenFireflies) {
        updateGoldenSpawning(dt);
    }

    // Top the meadow back up so there is always something to catch.
    while ((int)fireflies.size() < evening.fireflyCount) {
        spawnFirefly();
    }

    // The evening ends at full dark, or early once there's nothing left to light.
    if (timeLeft <= 0.0f || allLanternsLit()) {
        finishRound();
    }
}

void Game::drawEveningSelect() const
{
    ClearBackground(NIGHT_SKY_COLOR);
    drawCenteredText("Choose an evening", 50, TITLE_SIZE, TITLE_COLOR);

    if (evenings.empty()) {
        drawCenteredText("No evenings found.", 230, LINE_SIZE, TEXT_COLOR);
        drawCenteredText("Run the game from the project folder, next to evenings/.",
                         270, HUD_FONT_SIZE, HINT_COLOR);
        return;
    }

    for (int i = 0; i < (int)evenings.size(); i++) {
        int y = LIST_TOP + i * LIST_ROW_HEIGHT;
        if (i == selectedEvening) {
            Rectangle box = HIGHLIGHT_SIZE;
            box.y = (float)y - 8.0f;
            DrawRectangleRounded(box, 0.4f, 8, HIGHLIGHT_COLOR);
        }
        DrawText(TextFormat("%d. %s", i + 1, evenings[i].name.c_str()), LIST_LEFT, y, LINE_SIZE,
                 i == selectedEvening ? TITLE_COLOR : TEXT_COLOR);
        drawStarRow({(float)LIST_STARS_X, (float)y + LINE_SIZE / 2.0f - 1.0f}, bestStars[i]);
    }

    drawCenteredText("Up / Down to choose, Enter to play", GetScreenHeight() - 40,
                     HUD_FONT_SIZE, HINT_COLOR);
}

void Game::drawPlaying() const
{
    float progress = 1.0f - timeLeft / evening.length; // 0 at sunset, 1 at full dark
    ClearBackground(ColorLerp(SUNSET_SKY_COLOR, NIGHT_SKY_COLOR, progress));

    for (const Obstacle& obstacle : evening.obstacles) {
        obstacle.draw();
    }
    for (const Lantern& lantern : lanterns) {
        lantern.draw();
    }
    wind.draw();
    lightString.draw(); // drawn before the critters so they sit on top of its ends
    for (const std::unique_ptr<Firefly>& firefly : fireflies) {
        firefly->draw();
    }
    critter1.draw();
    critter2.draw();

    drawControlsHint();
    drawHud();
}

void Game::drawResults() const
{
    // Only the village under the night sky, so the lanterns you lit stand out.
    ClearBackground(NIGHT_SKY_COLOR);
    for (const Lantern& lantern : lanterns) {
        lantern.draw();
    }

    drawCenteredText(evening.name.c_str(), 110, HUD_FONT_SIZE, HINT_COLOR);
    const char* title = allLanternsLit() ? "Every lantern is lit!" : "Night has fallen";
    drawCenteredText(title, 140, TITLE_SIZE, TITLE_COLOR);
    drawCenteredText(TextFormat("Lanterns lit: %d of %d", countLitLanterns(), (int)lanterns.size()),
                     210, LINE_SIZE, TEXT_COLOR);
    drawCenteredText(TextFormat("Score: %d", score), 250, LINE_SIZE, TEXT_COLOR);
    drawCenteredText(TextFormat("Biggest delivery: %d %s", biggestDelivery,
                                biggestDelivery == 1 ? "firefly" : "fireflies"),
                     290, LINE_SIZE, TEXT_COLOR);

    float rowWidth = 2.0f * STAR_SPACING;
    drawStarRow({(GetScreenWidth() - rowWidth) / 2.0f, 350.0f}, starsEarned());

    drawCenteredText("Press Enter to continue", 410, HUD_FONT_SIZE, HINT_COLOR);
}

void Game::drawHud() const
{
    DrawText(TextFormat("Score %d", score), 16, 12, HUD_FONT_SIZE, HUD_TEXT_COLOR);

    // Round up so the clock shows 0:00 only once time has truly run out.
    int secondsLeft = (int)std::ceil(timeLeft);
    drawCenteredText(TextFormat("%d:%02d", secondsLeft / 60, secondsLeft % 60),
                     12, HUD_FONT_SIZE, HUD_TEXT_COLOR);
}

void Game::drawControlsHint() const
{
    // 1 while fully visible, then falling to 0 over the fade time.
    float elapsed = evening.length - timeLeft;
    float alpha = 1.0f - (elapsed - CONTROLS_HINT_SHOW_TIME) / CONTROLS_HINT_FADE_TIME;
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    if (alpha <= 0.0f) {
        return;
    }

    // Drawn in each critter's own color, so it's clear which keys move which one.
    drawTextBelow("WASD", critter1.getPosition(), Fade(CRITTER1_COLOR, alpha));
    drawTextBelow("Arrow keys", critter2.getPosition(), Fade(CRITTER2_COLOR, alpha));
}

void Game::spawnFirefly()
{
    const Rectangle& meadow = evening.meadow;
    Vector2 spawn = {
        (float)GetRandomValue((int)meadow.x, (int)(meadow.x + meadow.width)),
        (float)GetRandomValue((int)meadow.y, (int)(meadow.y + meadow.height)),
    };

    // Each kind's weight is its share of the total: with common 60, shy 40,
    // a roll of 1-40 is shy and 41-100 is common.
    int total = evening.commonWeight + evening.shyWeight + evening.pairWeight;
    int roll = GetRandomValue(1, total);
    if (roll <= evening.shyWeight) {
        fireflies.push_back(std::make_unique<ShyFirefly>(spawn));
    } else if (roll <= evening.shyWeight + evening.pairWeight) {
        fireflies.push_back(std::make_unique<PairFirefly>(spawn));
    } else {
        fireflies.push_back(std::make_unique<CommonFirefly>(spawn));
    }
}

void Game::updateGoldenSpawning(float dt)
{
    if (critter1.isMoving() || critter2.isMoving()) {
        stillTimer = 0.0f;
        return;
    }
    stillTimer += dt;
    if (stillTimer < GOLDEN_STILL_TIME) {
        return;
    }
    stillTimer = 0.0f;

    for (const std::unique_ptr<Firefly>& firefly : fireflies) {
        if (firefly->isGolden()) {
            return; // only one at a time
        }
    }

    // Beside the middle of the string, so it's clear the pause made it appear,
    // but not touching it, so it still has to be caught.
    Vector2 start = critter1.getPosition();
    Vector2 end = critter2.getPosition();
    Vector2 middle = Vector2Lerp(start, end, 0.5f);
    Vector2 along = Vector2Normalize(Vector2Subtract(end, start));
    Vector2 side = {-along.y, along.x}; // along the string, turned 90 degrees
    if (side.x == 0.0f && side.y == 0.0f) {
        side = {0.0f, -1.0f}; // critters on the same spot: just go above them
    }
    Vector2 spawn = Vector2Add(middle, Vector2Scale(side, GOLDEN_SPAWN_OFFSET));
    fireflies.push_back(std::make_unique<GoldenFirefly>(spawn));
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

bool Game::allLanternsLit() const
{
    return !lanterns.empty() && countLitLanterns() == (int)lanterns.size();
}

int Game::starsEarned() const
{
    // One for finishing, two for at least half the lanterns, three for all of them.
    int lit = countLitLanterns();
    if (allLanternsLit()) {
        return 3;
    }
    if (lit * 2 >= (int)lanterns.size()) {
        return 2;
    }
    return 1;
}
