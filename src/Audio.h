#pragma once

#include <vector>

#include "raylib.h"

// Every sound in the game, generated from sine waves when the game starts, so
// there are no audio files or licenses to manage. Needs the audio device open
// (InitAudioDevice) for as long as it exists.
class Audio {
public:
    Audio();
    ~Audio();

    // Each Audio owns its sounds and frees them when destroyed. A copy would
    // free the same sounds a second time, so copying isn't allowed.
    Audio(const Audio&) = delete;
    Audio& operator=(const Audio&) = delete;

    // A chime for the string's load reaching this size; bigger loads ring higher.
    void playCatch(int loadSize);

private:
    std::vector<Sound> chimes; // one per note, lowest first
};
