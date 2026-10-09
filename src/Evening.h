#pragma once

#include <sstream>
#include <string>
#include <vector>

#include "Obstacle.h"
#include "raylib.h"

// Everything that makes one evening different, read from a small text file in
// evenings/. Game builds a round from this, so a new evening needs only a new
// file, not new code. Every setting has a default, so a file only needs the
// lines that differ.
class Evening {
public:
    // Reads an evening file. Returns false (and logs why) if it can't be used.
    bool loadFromFile(const std::string& path);

    std::string name;
    float length = 180.0f;                    // seconds from sunset to full dark
    Vector2 critter1Start = {320.0f, 270.0f};
    Vector2 critter2Start = {640.0f, 270.0f};
    int fireflyCount = 12;                    // how many are out in the meadow at once
    Rectangle meadow = {40.0f, 40.0f, 560.0f, 460.0f}; // where fireflies appear
    // Relative chances of each kind for a new firefly.
    int commonWeight = 100;
    int shyWeight = 0;
    int pairWeight = 0;
    bool goldenFireflies = false;             // can golden ones appear?
    float windStrength = 0.0f;                // push at the height of a gust, px/s; 0 = calm
    float windDirection = 0.0f;               // degrees: 0 blows right, 90 down, 180 left
    std::vector<Vector2> lanterns;
    std::vector<Obstacle> obstacles;

private:
    // Applies one line's setting. Returns false if the line doesn't make sense.
    bool readSetting(const std::string& key, std::istringstream& values);
};
