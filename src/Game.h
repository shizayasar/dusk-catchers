#pragma once

#include <memory>
#include <vector>

#include "Audio.h"
#include "Critter.h"
#include "Evening.h"
#include "Firefly.h"
#include "Lantern.h"
#include "LightString.h"
#include "Particles.h"
#include "Wind.h"

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
    // Which screen is showing.
    enum class Screen { Title, EveningSelect, Playing, Paused, Results };

    void loadEvenings();
    void startRound();
    void finishRound();
    void updateTitle(float dt);
    void updateEveningSelect();
    void updatePlaying(float dt);
    void drawTitle() const;
    void drawEveningSelect() const;
    void drawPlaying() const;
    void drawPaused() const;
    void drawResults() const;
    void drawHud() const;
    void drawControlsHint() const;
    void spawnFirefly();
    void updateGoldenSpawning(float dt);
    void deliverToLanterns();
    int countLitLanterns() const;
    bool allLanternsLit() const;
    int starsEarned() const;

    std::vector<Evening> evenings;  // every evening found in the evenings folder
    std::vector<int> bestStars;     // best stars so far for each evening (0 = not played)
    int selectedEvening;            // index into evenings
    bool solo;                      // one player steering both critters
    std::vector<std::unique_ptr<Firefly>> titleFireflies; // drifting behind the title

    Evening evening; // the evening being played; everything below is built from it
    Critter critter1;
    Critter critter2;
    // Pointers, because each kind is a different subclass with its own size.
    // unique_ptr owns each firefly and frees it when it's removed from the list.
    std::vector<std::unique_ptr<Firefly>> fireflies;
    LightString lightString; // must come after the critters and fireflies it refers to
    std::vector<Lantern> lanterns;
    Wind wind;
    Particles particles;
    Audio audio;

    Screen screen;
    float timeLeft; // seconds until full dark
    int score;
    int biggestDelivery;
    float stillTimer; // how long both critters have been standing still
};
