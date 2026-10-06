#pragma once

#include "Firefly.h"

// The everyday firefly: drifts in slow loops and clings to any string it touches.
// All of that comes from Firefly; only its look is its own.
class CommonFirefly : public Firefly {
public:
    explicit CommonFirefly(Vector2 startPosition);

    void draw() const override;
};
