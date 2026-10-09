#pragma once

#include "raylib.h"

// Something in the way: critters can't walk through it, and the string snags if
// it crosses a tree or fence (it passes over water). Obstacles don't move or behave differently, they only differ
// in shape and look, so a simple kind enum is enough (unlike fireflies, which
// each behave differently and so are subclasses).
class Obstacle {
public:
    enum class Kind {
        Tree,  // a circle
        Fence, // a rectangle
        Water, // a rectangle the string can pass over
    };

    // Named constructors read more clearly than one constructor for every shape.
    static Obstacle tree(Vector2 center, float radius);
    static Obstacle fence(Rectangle area);
    static Obstacle water(Rectangle area);

    void draw() const;

    // Where a circle at `center` has to move so it no longer overlaps us.
    Vector2 pushOut(Vector2 center, float radius) const;
    // True if the string between these two points crosses us.
    bool snags(Vector2 stringStart, Vector2 stringEnd) const;

private:
    Obstacle(Kind kind, Vector2 center, float radius, Rectangle area);

    Kind kind;
    Vector2 center;  // trees only
    float radius;    // trees only
    Rectangle area;  // fences and water only
};
