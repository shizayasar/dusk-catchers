#pragma once

#include "raylib.h"

// Wooden planks drawn across a gap in the water. It doesn't block anything:
// critters can cross simply because there's no water under it.
class Bridge {
public:
    explicit Bridge(Rectangle area);

    void draw() const;

private:
    Rectangle area;
};
