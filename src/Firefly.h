#pragma once

#include "raylib.h"

class LightString; // only used by reference here, so the full classes aren't needed
class Obstacle;

// Base class for every kind of firefly. It holds what all kinds share: drifting
// in slow loops, clinging to the string, riding along and letting go. Each kind
// is a subclass that overrides only what makes it different.
class Firefly {
public:
    // Virtual so that destroying a firefly through a Firefly pointer also runs
    // the subclass's cleanup.
    virtual ~Firefly() = default;

    // Fireflies react to where the critters are, so those are passed in each
    // frame. The default just drifts; kinds that move differently override it.
    virtual void update(float dt, Vector2 critter1Position, Vector2 critter2Position);
    // Pure virtual: every kind must say how it looks.
    virtual void draw() const = 0;

    // How many fireflies this counts as in a load.
    virtual int getCount() const { return 1; }
    // How full the string's shake meter (0 to 1) can get before we fall off.
    virtual float getGrip() const { return 1.0f; }
    // Lets Game keep golden fireflies to one at a time.
    virtual bool isGolden() const { return false; }
    // Cling to the string if it touches us the way our kind needs.
    virtual void tryToCling(const LightString& string);
    // Move to our spot on the string, wherever its ends are now.
    virtual void followString(Vector2 stringStart, Vector2 stringEnd);
    // Fall off the string and go back to drifting.
    virtual void letGo();

    // Push a free firefly back out of an obstacle. Carried ones ride the string.
    void keepClearOf(const Obstacle& obstacle);

    bool isClinging() const { return clinging; }
    // False while clinging, and for a moment after being shaken loose.
    bool canBeCaught() const { return !clinging && catchCooldown <= 0.0f; }

protected:
    // Protected so only subclasses can be created; there's no plain "Firefly".
    explicit Firefly(Vector2 startPosition);

    static constexpr float DRIFT_SPEED = 35.0f; // pixels per second
    // Critters stop 18 pixels from the edge and the string catches within 10,
    // so anything closer to the edge than 28 pixels would be out of reach.
    static constexpr float EDGE_MARGIN = 24.0f;

    // Tick the timers and turn a little; call once per frame while free.
    void wander(float dt);
    // Where `from` ends up after moving along our heading, kept on screen.
    Vector2 moveForward(Vector2 from, float speed, float dt) const;
    // `point`, pushed back inside the screen so it stays margin pixels from every edge.
    static Vector2 keepOnScreen(Vector2 point, float margin);
    static bool touches(const LightString& string, Vector2 point);

    // The middle of the firefly and how far it reaches from there. Pairs
    // override these so both bodies are kept clear, not just one.
    virtual Vector2 getCenter() const { return position; }
    virtual float getReach() const;
    // Also keeps the firefly on screen, wherever it's moved to.
    virtual void moveCenterTo(Vector2 center) { position = keepOnScreen(center, EDGE_MARGIN); }
    static void drawBody(Vector2 at, Color core, Color halo);
    static float randomFloat(float min, float max);

    Vector2 position;
    float heading;       // direction of travel, in radians
    bool clinging;
    float stringT;       // where along the string we cling, from 0 to 1
    float catchCooldown; // seconds until we can be caught again after letting go

private:
    void pickNewTurnRate();

    float turnRate;      // radians per second; the sign decides left or right loops
    float turnTimer;     // seconds until we pick a new turn rate
};
