# Dusk Catchers

A cozy two-player co-op game in C++17 with raylib, built as a portfolio project and published as a browser game. The full game design, milestones and learning goals are in DESIGN.md:

@DESIGN.md

## About me

- I know C++ basics and object-oriented programming from one class. I'm learning as we build.
- Explain what you're about to do and why, in plain language, before each change.
- When you finish a milestone, summarize what each new file and class does so I can explain it in an interview.

## How we work

- Work on one milestone from DESIGN.md at a time. Don't start the next one until I say so.
- Keep changes small. Build and run the game after every change to confirm it still works.
- Commit to git after each working step, with a clear commit message.
- Don't add features that aren't in DESIGN.md. If you have an idea, suggest it and wait for my answer.
- When a milestone is done, tick its box in DESIGN.md. If we change the plan, update DESIGN.md so it stays accurate.

## Code style

- C++17, raylib and CMake, with raylib downloaded through FetchContent.
- Simple classes, each with `update(float dt)` and `draw()`. Multiply all movement by `dt`.
- One class per `.h`/`.cpp` pair, in `src/`.
- Prefer clear code over clever code. Comments explain why, not what.
- No raw `new` or `delete`; use plain values, `std::vector` or `std::unique_ptr`.
- Keep tuning numbers (speeds, string length, timers) as named constants near the top of a file so they're easy to adjust.

## Platform

- I'm on a Mac and use VS Code.
- Once the project builds, add the exact build and run commands for macOS to this file.

## Build and run (macOS)

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug   # configure; first time only (downloads raylib)
cmake --build build                            # compile after every change
./build/dusk_catchers                          # run the game
```
