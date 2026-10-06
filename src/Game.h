#pragma once

#include <memory>
#include <vector>

#include "Critter.h"
#include "Firefly.h"
#include "Lantern.h"
#include "LightString.h"

// Owns everything in the game, keeps score and switches between screens.
// main() calls update() then draw() once per frame.
class Game {
public:
    Game();

    // dt is the time in seconds since the last frame. Multiplying movement by dt
    // keeps speeds the same no matter how fast the computer runs.
    void update(float dt);
    void draw();

private:
    // Which screen is showing. Title and evening select will join these later.
    enum class Screen { Playing, Results };

    void startRound();
    void updatePlaying(float dt);
    void drawPlaying() const;
    void drawResults() const;
    void drawHud() const;
    void drawControlsHint() const;
    void spawnFirefly();
    void updateGoldenSpawning(float dt);
    void deliverToLanterns();
    int countLitLanterns() const;

    Critter critter1;
    Critter critter2;
    // Pointers, because each kind is a different subclass with its own size.
    // unique_ptr owns each firefly and frees it when it's removed from the list.
    std::vector<std::unique_ptr<Firefly>> fireflies;
    LightString lightString; // must come after the critters and fireflies it refers to
    std::vector<Lantern> lanterns;

    Screen screen;
    float timeLeft; // seconds until full dark
    int score;
    int biggestDelivery;
    float stillTimer; // how long both critters have been standing still
};
