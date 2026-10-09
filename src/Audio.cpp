#include "Audio.h"

#include <algorithm>
#include <cmath>

namespace {
const int SAMPLE_RATE = 44100; // samples per second

// Chimes.
const float CHIME_LENGTH = 1.2f;        // seconds, including the ring-out
const float CHIME_ATTACK = 0.005f;      // seconds to fade in, so it doesn't click
const float CHIME_DECAY = 4.0f;         // how fast it dies away; higher is shorter
const float CHIME_VOLUME = 0.25f;       // leaves headroom when chimes overlap
const float CHIME_LOWEST_NOTE = 523.25f; // Hz (C5)
// Steps up a major pentatonic scale, in semitones above the lowest note. Any
// two of these notes sound nice together, so overlapping chimes never clash.
const int CHIME_SEMITONES[] = {0, 2, 4, 7, 9, 12, 14, 16, 19, 21, 24, 26};

// Turns floating-point samples (-1 to 1) into a raylib Sound.
Sound soundFromSamples(const std::vector<float>& samples)
{
    std::vector<short> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        float clamped = std::clamp(samples[i], -1.0f, 1.0f);
        pcm[i] = (short)(clamped * 32767.0f); // 16-bit audio runs from -32768 to 32767
    }
    Wave wave;
    wave.frameCount = (unsigned int)pcm.size();
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = pcm.data();
    return LoadSoundFromWave(wave); // copies the data, so pcm can go away afterwards
}

// A bell-like tone: the note plus two quieter harmonics, with a quick fade-in
// and a long, smooth fade-out.
std::vector<float> makeChime(float frequency)
{
    int count = (int)(CHIME_LENGTH * SAMPLE_RATE);
    std::vector<float> samples(count);
    for (int i = 0; i < count; i++) {
        float t = (float)i / SAMPLE_RATE;
        float tone = std::sin(2.0f * PI * frequency * t)
                   + 0.3f * std::sin(2.0f * PI * 2.0f * frequency * t)
                   + 0.1f * std::sin(2.0f * PI * 3.0f * frequency * t);
        float fadeIn = std::min(1.0f, t / CHIME_ATTACK);
        float fadeOut = std::exp(-CHIME_DECAY * t);
        samples[i] = CHIME_VOLUME * tone * fadeIn * fadeOut / 1.4f; // 1.4 = loudest the three waves can add to
    }
    return samples;
}
}

Audio::Audio()
{
    for (int semitones : CHIME_SEMITONES) {
        // Each semitone multiplies the frequency by the twelfth root of 2.
        float frequency = CHIME_LOWEST_NOTE * std::pow(2.0f, semitones / 12.0f);
        chimes.push_back(soundFromSamples(makeChime(frequency)));
    }
}

Audio::~Audio()
{
    for (Sound& chime : chimes) {
        UnloadSound(chime);
    }
}

void Audio::playCatch(int loadSize)
{
    if (chimes.empty() || loadSize < 1) {
        return;
    }
    int note = std::min(loadSize, (int)chimes.size()) - 1;
    PlaySound(chimes[note]);
}
