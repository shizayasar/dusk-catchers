#include "Progress.h"

#include <sstream>

#include "raylib.h"

#if defined(__EMSCRIPTEN__)
#include <emscripten/emscripten.h>
#endif

namespace {
// One line per evening, stars first so the name can contain spaces: "3 First light".
std::string toText(const std::map<std::string, int>& bestStars)
{
    std::string text;
    for (const auto& [name, stars] : bestStars) {
        text += std::to_string(stars) + " " + name + "\n";
    }
    return text;
}

std::map<std::string, int> fromText(const std::string& text)
{
    std::map<std::string, int> bestStars;
    std::istringstream lines(text);
    std::string line;
    while (std::getline(lines, line)) {
        std::istringstream words(line);
        int stars = 0;
        std::string name;
        if (words >> stars && std::getline(words >> std::ws, name) && !name.empty()) {
            bestStars[name] = stars;
        }
    }
    return bestStars;
}

#if defined(__EMSCRIPTEN__)
const char* const STORAGE_KEY = "dusk-catchers-stars"; // the localStorage entry

// The saved text goes inside a line of JavaScript as a quoted string, so any
// quote, backslash or line break in it has to be escaped.
std::string asJavaScriptString(const std::string& text)
{
    std::string quoted = "'";
    for (char c : text) {
        if (c == '\'' || c == '\\') {
            quoted += '\\';
            quoted += c;
        } else if (c == '\n') {
            quoted += "\\n";
        } else {
            quoted += c;
        }
    }
    return quoted + "'";
}
#else
const char* const SAVE_FILE = "stars.txt"; // next to the game program

std::string saveFilePath()
{
    return std::string(GetApplicationDirectory()) + SAVE_FILE;
}
#endif
}

std::map<std::string, int> Progress::load()
{
#if defined(__EMSCRIPTEN__)
    // C++ can't reach localStorage directly, so ask the browser to run a line of
    // JavaScript. The try/catch covers browsers that block storage (some
    // private modes): we just start with no saved stars.
    std::string script = std::string("(function(){try{return localStorage.getItem('") + STORAGE_KEY
                       + "')||'';}catch(e){return '';}})()";
    return fromText(emscripten_run_script_string(script.c_str()));
#else
    std::string path = saveFilePath();
    if (!FileExists(path.c_str())) {
        return {};
    }
    char* text = LoadFileText(path.c_str());
    if (text == nullptr) {
        return {};
    }
    std::map<std::string, int> bestStars = fromText(text);
    UnloadFileText(text); // raylib allocated it; hand it back
    return bestStars;
#endif
}

void Progress::save(const std::map<std::string, int>& bestStars)
{
    std::string text = toText(bestStars);
#if defined(__EMSCRIPTEN__)
    std::string script = std::string("try{localStorage.setItem('") + STORAGE_KEY + "',"
                       + asJavaScriptString(text) + ");}catch(e){}";
    emscripten_run_script(script.c_str());
#else
    SaveFileText(saveFilePath().c_str(), text.data());
#endif
}
