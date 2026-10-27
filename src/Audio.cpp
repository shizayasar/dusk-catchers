#include "Audio.h"

#include <algorithm>
#include <cmath>
#include <ctime>

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

// Lantern lighting: a rising run of bell notes from one chord. A brighter
// lantern (a bigger delivery) plays more of the run, so it sounds fuller.
const float LANTERN_NOTES[] = {261.63f, 392.00f, 523.25f, 659.25f, 783.99f, 1046.50f}; // C G C E G C
const float LANTERN_NOTE_GAP = 0.08f;     // seconds between one note starting and the next
const float LANTERN_DECAY = 2.2f;         // slower than a chime, so it rings out longer
const float LANTERN_RING_LENGTH = 2.2f;   // seconds the last note is given to ring
const float LANTERN_NOTE_VOLUME = 0.13f;  // per note; six notes together stay well below clipping
const int FIREFLIES_PER_LANTERN_NOTE = 2; // each two extra fireflies add a note

// Ambient music: a loop of soft chords and crickets.
const float MUSIC_LOOP_LENGTH = 16.0f; // seconds
const float PAD_VOLUME = 0.035f;      // per note; six notes share the pad
// Two chords that slowly trade places over the loop (C major and A minor).
const float CHORD_A[] = {130.81f, 196.00f, 329.63f};
const float CHORD_B[] = {110.00f, 164.81f, 261.63f};
const float CRICKET_PITCH = 4500.0f;  // Hz
const float CRICKET_VOLUME = 0.035f;
const int CRICKET_CHIRPS = 14;        // chirps per loop
const int CRICKET_PULSES = 3;         // a chirp is a few quick pulses
const float CRICKET_PULSE_LENGTH = 0.018f; // seconds
const float CRICKET_PULSE_GAP = 0.03f;     // seconds from one pulse to the next
const unsigned int CRICKET_SEED = 7;  // fixed, so the loop sounds the same every run
const int MUSIC_CHUNK = 4096;         // samples handed to the stream at a time

// 16-bit audio runs from -32768 to 32767.
std::vector<short> toPcm(const std::vector<float>& samples)
{
    std::vector<short> pcm(samples.size());
    for (size_t i = 0; i < samples.size(); i++) {
        pcm[i] = (short)(std::clamp(samples[i], -1.0f, 1.0f) * 32767.0f);
    }
    return pcm;
}

// Turns floating-point samples (-1 to 1) into a raylib Sound.
Sound soundFromSamples(const std::vector<float>& samples)
{
    std::vector<short> pcm = toPcm(samples);
    Wave wave;
    wave.frameCount = (unsigned int)pcm.size();
    wave.sampleRate = SAMPLE_RATE;
    wave.sampleSize = 16;
    wave.channels = 1;
    wave.data = pcm.data();
    return LoadSoundFromWave(wave); // copies the data, so pcm can go away afterwards
}

// Adds one bell-like note into `samples`, starting `start` seconds in: the
// note plus two quieter harmonics, with a quick fade-in and a smooth fade-out.
void addBell(std::vector<float>& samples, float frequency, float start, float volume, float decay)
{
    int first = (int)(start * SAMPLE_RATE);
    for (int i = first; i < (int)samples.size(); i++) {
        float t = (float)(i - first) / SAMPLE_RATE;
        float tone = std::sin(2.0f * PI * frequency * t)
                   + 0.3f * std::sin(2.0f * PI * 2.0f * frequency * t)
                   + 0.1f * std::sin(2.0f * PI * 3.0f * frequency * t);
        float fadeIn = std::min(1.0f, t / CHIME_ATTACK);
        float fadeOut = std::exp(-decay * t);
        samples[i] += volume * tone * fadeIn * fadeOut / 1.4f; // 1.4 = loudest the three waves can add to
    }
}

std::vector<float> makeChime(float frequency)
{
    std::vector<float> samples((int)(CHIME_LENGTH * SAMPLE_RATE), 0.0f);
    addBell(samples, frequency, 0.0f, CHIME_VOLUME, CHIME_DECAY);
    return samples;
}

// The first `noteCount` notes of the lantern run, each starting a moment
// after the last so the sound blooms open like the glow.
std::vector<float> makeLanternSound(int noteCount)
{
    float length = (noteCount - 1) * LANTERN_NOTE_GAP + LANTERN_RING_LENGTH;
    std::vector<float> samples((int)(length * SAMPLE_RATE), 0.0f);
    for (int n = 0; n < noteCount; n++) {
        addBell(samples, LANTERN_NOTES[n], n * LANTERN_NOTE_GAP, LANTERN_NOTE_VOLUME, LANTERN_DECAY);
    }
    return samples;
}

// For the loop to join up without a click, every wave has to finish a whole
// number of cycles in it. Nudging each frequency to the nearest multiple of
// 1/loop-length Hz does that; the change is far too small to hear.
float loopSafe(float frequency)
{
    return std::round(frequency * MUSIC_LOOP_LENGTH) / MUSIC_LOOP_LENGTH;
}

std::vector<float> makeMusic()
{
    int count = (int)(MUSIC_LOOP_LENGTH * SAMPLE_RATE);
    std::vector<float> samples(count, 0.0f);

    for (int i = 0; i < count; i++) {
        float t = (float)i / SAMPLE_RATE;
        // One full cosine cycle per loop: chord A fades into B and back.
        float mixA = 0.5f + 0.5f * std::cos(2.0f * PI * t / MUSIC_LOOP_LENGTH);
        float pad = 0.0f;
        for (int n = 0; n < 3; n++) {
            pad += mixA * std::sin(2.0f * PI * loopSafe(CHORD_A[n]) * t);
            pad += (1.0f - mixA) * std::sin(2.0f * PI * loopSafe(CHORD_B[n]) * t);
        }
        samples[i] = PAD_VOLUME * pad;
    }

    // Crickets: each chirp is a few short pulses of a high tone, at random
    // (but repeatable) moments through the loop.
    SetRandomSeed(CRICKET_SEED);
    float chirpLength = CRICKET_PULSES * CRICKET_PULSE_GAP;
    for (int c = 0; c < CRICKET_CHIRPS; c++) {
        float start = GetRandomValue(0, (int)((MUSIC_LOOP_LENGTH - chirpLength) * 1000)) / 1000.0f;
        for (int p = 0; p < CRICKET_PULSES; p++) {
            int first = (int)((start + p * CRICKET_PULSE_GAP) * SAMPLE_RATE);
            int length = (int)(CRICKET_PULSE_LENGTH * SAMPLE_RATE);
            for (int k = 0; k < length && first + k < count; k++) {
                float t = (float)k / SAMPLE_RATE;
                float shape = std::sin(PI * k / length); // swell and fade, no clicks
                samples[first + k] += CRICKET_VOLUME * shape * std::sin(2.0f * PI * CRICKET_PITCH * t);
            }
        }
    }
    // Put the shared random generator back to an unpredictable seed for gameplay.
    SetRandomSeed((unsigned int)std::time(nullptr));
    return samples;
}
}

Audio::Audio()
    : music(toPcm(makeMusic())),
      musicPosition(0)
{
    SetAudioStreamBufferSizeDefault(MUSIC_CHUNK);
    musicStream = LoadAudioStream(SAMPLE_RATE, 16, 1);
    PlayAudioStream(musicStream);

    for (int semitones : CHIME_SEMITONES) {
        // Each semitone multiplies the frequency by the twelfth root of 2.
        float frequency = CHIME_LOWEST_NOTE * std::pow(2.0f, semitones / 12.0f);
        chimes.push_back(soundFromSamples(makeChime(frequency)));
    }

    int lanternNoteCount = sizeof(LANTERN_NOTES) / sizeof(LANTERN_NOTES[0]);
    for (int notes = 1; notes <= lanternNoteCount; notes++) {
        lanternSounds.push_back(soundFromSamples(makeLanternSound(notes)));
    }
}

Audio::~Audio()
{
    UnloadAudioStream(musicStream);
    for (Sound& chime : chimes) {
        UnloadSound(chime);
    }
    for (Sound& sound : lanternSounds) {
        UnloadSound(sound);
    }
}

void Audio::update()
{
    // Whenever the stream has used up a chunk, hand it the next one, wrapping
    // round to the start of the loop when we reach the end.
    while (IsAudioStreamProcessed(musicStream)) {
        short chunk[MUSIC_CHUNK];
        for (int i = 0; i < MUSIC_CHUNK; i++) {
            chunk[i] = music[musicPosition];
            musicPosition = (musicPosition + 1) % music.size();
        }
        UpdateAudioStream(musicStream, chunk, MUSIC_CHUNK);
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

void Audio::playLanternLit(int fireflyCount)
{
    if (lanternSounds.empty() || fireflyCount < 1) {
        return;
    }
    // 1 firefly plays one note, 2-3 play two, 4-5 three, ... and 10 or more
    // (enough for the big lantern) play the full run.
    int notes = fireflyCount / FIREFLIES_PER_LANTERN_NOTE + 1;
    notes = std::clamp(notes, 1, (int)lanternSounds.size());
    PlaySound(lanternSounds[notes - 1]);
}
