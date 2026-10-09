# Dusk Catchers — Design Doc

## Pitch

Two tiny critters share a glowing string of light. Fireflies cling to it as it brushes past, and the critters must carry them home together, gently, to light the lanterns of a sleeping village. It is a cozy, no-fail co-op game for one keyboard, playable in the browser, built in C++ to show off game feel and local co-op.

Target: a polished 3-minute experience published on itch.io and linked from your portfolio.

## Core loop

Each evening is one round, about 3 minutes from sunset to full dark.

1. Move the two critters so the string brushes through fireflies; each one clings to it.
2. Carry the load to an unlit lantern, moving gently so nobody falls off.
3. Sweep the string across the lantern to deliver. Bigger loads light it brighter and score more.
4. When night falls, or as soon as every lantern is lit, the village shows every lantern you lit, your score and your biggest delivery.

Each evening earns one to three stars and can be replayed. Nobody loses; the village is just brighter or dimmer.

## Mechanics

The whole game rests on one rule: fireflies cling to the string, and you have to get them home together.

- **The string.** A glowing line joins the two critters and moves with them. Any firefly it touches clings to it and rides along.
- **Shaking loose.** Yank the string by moving the critters in different directions (pulling apart or swinging it sharply), or stretch it past the maximum length, and fireflies start dropping off one by one. Moving both critters together, even at full speed, is safe. The string flickers first as a warning.
- **Delivering.** Sweep the string across an unlit lantern to hand over every firefly on it. A bigger load scores more and lights the lantern brighter.
- **The village.** Lit lanterns stay lit and brighten their corner of the map for the rest of the evening.

| Firefly | How it behaves | What it asks of players |
| --- | --- | --- |
| Common | Drifts in slow loops | Learn to gather and carry |
| Shy | Darts away when a critter gets close | Brush past from a distance, using the string's length |
| Pair | Two linked fireflies that only cling if gathered in the same sweep | Line up one clean sweep together |
| Golden | Appears only after both critters stand still together for 2 seconds, shakes loose easily, and counts as 3 fireflies in a load | Pause together, then carry extra gently |

## Evenings

The game is a week of seven short evenings. Each one adds a single new idea, so it gets harder without piling on rules.

| Evening | New idea | What it asks of players |
| --- | --- | --- |
| 1. First light | Open meadow, common fireflies, village close by | Learn to gather and carry |
| 2. Shy ones | Shy fireflies | Brush past from a distance |
| 3. Partners | Pair fireflies | Line up one clean sweep together |
| 4. The long way home | Trees and fences between the field and the village | Thread the string around obstacles |
| 5. Breezy night | Gusts that tug the string | Carry against the wind |
| 6. The footbridge | A narrow bridge over a stream | Cross single file, carefully |
| 7. Festival night | Everything at once, plus golden fireflies | Light the big lantern in the town square |

- **Stars, not failure.** Each evening ends at full dark however you did, or early once every lantern is lit, and earns one to three stars for the lanterns you lit: one star for finishing, two for lighting at least half, three for lighting them all. Best stars are remembered while the game is open; saving them between sessions waits for the web build (milestone 8).
- **Obstacles.** Critters can't walk through trees, fences or water. If the string crosses a tree or fence it snags and fireflies start shaking loose, so both critters must go around the same side.
- **Wind.** Every few seconds a gust blows, shown as streaks. It pushes both critters along and strains the string like a sail: a long string side-on to the wind strains most, a short one pointing into the wind barely at all.
- **The big lantern.** Festival night's lantern in the town square only lights from a single delivery of at least 10 fireflies.
- **Everything unlocked.** All evenings are open from the start, so a visitor with two minutes can jump straight to the best one.
- **Data, not code.** Each evening is a small data file listing its firefly types, obstacles, wind strength and lantern positions. Adding an evening means writing a file, not new C++.

## Modes and controls

Co-op and solo use the same keys, so solo is simply one person steering both critters with both hands.

| Mode | Critter 1 | Critter 2 | Tuning |
| --- | --- | --- | --- |
| Co-op, one keyboard | WASD | Arrow keys | Standard |
| Co-op, gamepads | Gamepad 1 left stick | Gamepad 2 left stick | Standard |
| Solo, both hands | WASD | Arrow keys | Longer string, fireflies cling more tightly |

Enter starts a round and Esc pauses. For the first few seconds of each round, each critter's keys are shown beneath it and then fade out. Browsers only detect a gamepad after a button press, so the title screen says "Press any button to join." Solo matters most for the portfolio, since most visitors will play alone.

## Look, feel and sound

The look is built from shapes and light, so it can look pretty before there is any art.

- **Palette.** The sky fades from dusky purple to deep navy over the round; fireflies and the string glow warm yellow, and lit lanterns bloom orange across the village.
- **Glow.** Draw each firefly and the string as a bright core with a few soft, transparent halos, using additive blending so they bloom against the dark.
- **Critters.** Start as round shapes with dot eyes. Swap in sprites from a free asset pack or original pixel art once the game is fun.
- **Juice.** Each firefly that clings gives a tiny chime that rises in pitch as the load grows. A delivery bursts into particles as the lantern blooms open. Critters squash and stretch a little as they move.
- **Sound.** One calm ambient loop (crickets, soft pads) plus the catch chimes. Use only assets we are licensed to use, and credit them in the README.

## Tech plan

The stack is C++17, raylib, CMake and Emscripten, with git and GitHub for history and itch.io for hosting. raylib is a small library of plain functions like `DrawCircle()`, which keeps the code readable for a beginner, and CMake downloads it automatically.

The code is a handful of classes, each with `update(dt)` and `draw()`; there is no engine architecture.

| Class | Count | Responsibilities |
| --- | --- | --- |
| `Game` | 1 | Owns every object. Loads each evening from data and keeps score. Each frame: input, then update, then draw. Switches screens: Title, Evening select, Playing, Results. |
| `Critter` | 2 | Position and speed. Reads its own keys or gamepad. |
| `LightString` | 1 | The line between the two critters. Picks up fireflies it touches (line-vs-circle test), carries them, drops them if shaken, and delivers its load to a lantern. |
| `Firefly` | many | Has a kind (common, shy, pair, golden) and moves according to its kind. |
| `Lantern` | many | Lit or unlit; its glow grows with the size of the load delivered. |

The string does most of the work: it reads the critters' positions, picks up any firefly it touches, drops them when shaken, and hands its load to a lantern.

## Milestones

Plan on roughly 5 to 7 weeks of part-time work; milestones 1 to 3 fit in a weekend. Each one ends with something playable, so commit to git every time one works.

- [x] **1. Window on screen.** CMake builds the project and raylib opens a dark 960×540 window. Done when it runs on the Mac.
- [x] **2. Two critters move.** WASD and the arrow keys move two circles at a frame-rate-independent speed. Done when both move smoothly and stay on screen.
- [x] **3. Gather and carry.** The glowing line joins the critters, common fireflies drift, and any firefly the string touches clings to it. Done when fireflies stay on the string as it moves and stretches.
- [x] **4. Deliver and light up.** Lanterns to deliver to, a 3-minute timer, a darkening sky, and a results screen showing the lit village. Done when a friend can play start to finish without explanation.
- [x] **5. Variety.** Shy, pair and golden fireflies, plus shaking loose when you move too fast or stretch too far. Done when carrying a big load feels tense but fair.
- [ ] **6. Seven evenings.** Load each evening from a data file, build all seven, and add an evening-select screen with stars. Done when a new evening can be added without changing any C++.
- [ ] **7. Polish.** Glow, particles, chimes, music, a title screen and solo tuning. Done when three people have played it and their top complaints are fixed.
- [ ] **8. Ship it.** Emscripten web build on itch.io, a README with a gameplay GIF, and a GitHub Actions build. Done when the link works in a fresh browser.

## Scope and learning goals

Left out on purpose: online multiplayer, accounts, leaderboards, story, and any custom engine. New ideas go on a "later" list, not into the build.

The developer should be able to explain each of these in an interview:

- **Game loop and delta time.** Why movement is multiplied by `dt`, so speed is the same at any frame rate.
- **Line-vs-circle collision.** How the string decides it touched a firefly or a lantern (distance from a point to a line segment).
- **Carrying.** How the string tracks its speed and stretch each frame to decide when a firefly falls off.
- **Firefly kinds, before and after.** Start with an `enum` and a `switch`, then refactor to a `Firefly` base class with a virtual `update()` once there are three kinds. This is a good story to tell about OOP choices.
- **Data-driven evenings.** How the game reads an evening from a file and builds it, so new evenings need no new code.
- **Screen states.** How the game switches between Title, Evening select, Playing and Results.
- **Web builds.** What Emscripten does to turn C++ into something a browser can run.
