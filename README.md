# Dusk Catchers

A cozy two-player co-op game: two critters share a glowing string, gather fireflies on it, and carry them home to light the lanterns of a sleeping village. Built in C++17 with [raylib](https://www.raylib.com/).

Work in progress. See [DESIGN.md](DESIGN.md) for the full plan.

## Build and run (macOS)

You need the Xcode Command Line Tools and CMake:

```sh
xcode-select --install
brew install cmake
```

Then, from the project folder:

```sh
cmake -S . -B build            # first time only; downloads raylib (takes about a minute)
cmake --build build            # compile
./build/dusk_catchers          # run
```

After changing code, only the last two commands are needed.
