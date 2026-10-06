#pragma once

#include "Firefly.h"

// A rare prize that appears when both critters stand still together. It counts
// as three fireflies but has a weak grip, so it slips off the string early.
class GoldenFirefly : public Firefly {
public:
    explicit GoldenFirefly(Vector2 startPosition);

    void draw() const override;

    int getCount() const override;
    float getGrip() const override;
    bool isGolden() const override { return true; }
};
