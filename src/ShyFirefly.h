#pragma once

#include "Firefly.h"

// Darts away when a critter gets close, so it has to be brushed with the middle
// of the string, from a distance.
class ShyFirefly : public Firefly {
public:
    explicit ShyFirefly(Vector2 startPosition);

    void update(float dt, Vector2 critter1Position, Vector2 critter2Position) override;
    void draw() const override;
};
