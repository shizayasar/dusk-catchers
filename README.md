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

The game looks for its evenings in `evenings/`, so run it from the project folder.

## Adding an evening

Each evening is a text file in [`evenings/`](evenings/). The game loads every `.txt` file there, in filename order, so name a new one with the next number (for example `08-moonrise.txt`). No code changes or rebuild are needed.

Write one setting per line: a keyword, then its numbers. Anything after `#` is a comment. Every setting except `name` and at least one lantern has a default, so a file only needs the lines that differ. Positions are in pixels on the 960×540 screen.

| Line | Meaning | Default |
| --- | --- | --- |
| `name First light` | The name shown on the evening-select screen. Required. | — |
| `length 180` | Seconds from sunset to full dark. | `180` |
| `critters 320 270 640 270` | Where critter 1 and critter 2 start (x y x y). | `320 270 640 270` |
| `fireflies 12` | How many fireflies are out at once. | `12` |
| `meadow 40 40 560 460` | The area where fireflies appear (x y width height). | `40 40 560 460` |
| `mix common 60 shy 25 pair 15` | Relative chances of each firefly kind. | `common 100` |
| `golden` | Golden fireflies appear when both critters stand still. | off |
| `wind 90 180` | Gust strength (pixels per second) and direction in degrees (0 right, 90 down, 180 left, 270 up). | calm |
| `lantern 720 100` | A lantern (x y). Repeat for each one. At least one is required. | — |
| `biglantern 770 270 10` | A big lantern that needs a single delivery of at least this many fireflies (x y needed). | — |
| `tree 470 110 30` | A tree (x y radius). Blocks critters and snags the string. | — |
| `fence 560 0 16 150` | A fence (x y width height). Blocks critters and snags the string. | — |
| `water 560 0 80 240` | Water (x y width height). Blocks critters; the string passes over it. | — |
| `bridge 548 240 104 60` | Planks drawn over a gap left between two `water` areas (x y width height). | — |

If a line can't be read, the game skips it and logs a warning with the file name and line number.
