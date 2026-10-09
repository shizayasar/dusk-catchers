#include "Evening.h"

#include <fstream>

bool Evening::loadFromFile(const std::string& path)
{
    std::ifstream file(path);
    if (!file) {
        TraceLog(LOG_WARNING, "EVENING: Couldn't open %s", path.c_str());
        return false;
    }

    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = line.substr(0, line.find('#')); // everything after # is a comment

        std::istringstream values(line);
        std::string key;
        if (!(values >> key)) {
            continue; // blank line
        }
        // A bad line is skipped rather than failing the whole evening, but logged
        // with its line number so it's easy to find and fix.
        if (!readSetting(key, values)) {
            TraceLog(LOG_WARNING, "EVENING: %s line %d: couldn't understand \"%s\"",
                     path.c_str(), lineNumber, line.c_str());
        }
    }

    if (name.empty() || lanterns.empty()) {
        TraceLog(LOG_WARNING, "EVENING: %s needs a name and at least one lantern", path.c_str());
        return false;
    }
    return true;
}

bool Evening::readSetting(const std::string& key, std::istringstream& values)
{
    // `values >> x` reads the next word into x and is false if that fails, so
    // each setting succeeds only if all of its numbers were there.
    if (key == "name") {
        std::getline(values >> std::ws, name); // the rest of the line, spaces and all
        return !name.empty();
    }
    if (key == "length") {
        return static_cast<bool>(values >> length);
    }
    if (key == "critters") {
        return static_cast<bool>(values >> critter1Start.x >> critter1Start.y
                                        >> critter2Start.x >> critter2Start.y);
    }
    if (key == "fireflies") {
        return static_cast<bool>(values >> fireflyCount);
    }
    if (key == "meadow") {
        return static_cast<bool>(values >> meadow.x >> meadow.y >> meadow.width >> meadow.height);
    }
    if (key == "mix") {
        commonWeight = shyWeight = pairWeight = 0;
        std::string kind;
        int weight = 0;
        while (values >> kind >> weight) {
            if (kind == "common") {
                commonWeight = weight;
            } else if (kind == "shy") {
                shyWeight = weight;
            } else if (kind == "pair") {
                pairWeight = weight;
            } else {
                return false;
            }
        }
        return commonWeight + shyWeight + pairWeight > 0;
    }
    if (key == "golden") {
        goldenFireflies = true;
        return true;
    }
    if (key == "lantern") {
        Vector2 position;
        if (!(values >> position.x >> position.y)) {
            return false;
        }
        lanterns.push_back(position);
        return true;
    }
    if (key == "wind") {
        return static_cast<bool>(values >> windStrength >> windDirection);
    }
    if (key == "tree") {
        Vector2 center;
        float radius = 0.0f;
        if (!(values >> center.x >> center.y >> radius)) {
            return false;
        }
        obstacles.push_back(Obstacle::tree(center, radius));
        return true;
    }
    if (key == "fence") {
        Rectangle area;
        if (!(values >> area.x >> area.y >> area.width >> area.height)) {
            return false;
        }
        obstacles.push_back(Obstacle::fence(area));
        return true;
    }
    if (key == "water" || key == "bridge") {
        Rectangle area;
        if (!(values >> area.x >> area.y >> area.width >> area.height)) {
            return false;
        }
        if (key == "water") {
            obstacles.push_back(Obstacle::water(area));
        } else {
            bridges.push_back(Bridge(area));
        }
        return true;
    }
    return false; // not a setting we know
}
