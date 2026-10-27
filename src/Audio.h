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

    // Keeps the ambient music fed; call once per frame.
    void update();

    // A chime for the string's load reaching this size; bigger loads ring higher.
    void playCatch(int loadSize);
    // A lantern lighting up; bigger deliveries (brighter lanterns) sound fuller.
    void playLanternLit(int fireflyCount);

private:
    std::vector<Sound> chimes; // one per note, lowest first
    std::vector<Sound> lanternSounds; // the lantern run with 1, 2, ... notes

    // The ambient loop, played by handing the stream one chunk at a time.
    std::vector<short> music;
    size_t musicPosition;      // next sample of `music` to hand over
    AudioStream musicStream;
};
