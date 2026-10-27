#pragma once

#include <map>
#include <string>

// Remembers the best stars earned on each evening between sessions: in a small
// file on desktop, and in the browser's local storage on the web. Stars are
// stored by evening name, so adding or reordering evening files never mixes
// them up.
class Progress {
public:
    // Best stars by evening name. Empty if nothing has been saved yet.
    static std::map<std::string, int> load();
    static void save(const std::map<std::string, int>& bestStars);
};
