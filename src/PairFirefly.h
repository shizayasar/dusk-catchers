#pragma once

#include "Firefly.h"

// Two linked fireflies whose link slowly spins. They only cling if the string
// touches both bodies at once, so players must line the string up with the
// link. They count as two fireflies and fall off together.
//
// `position` (from Firefly) is the first body; the second body is the partner.
class PairFirefly : public Firefly {
public:
    explicit PairFirefly(Vector2 startPosition);

    void update(float dt, Vector2 critter1Position, Vector2 critter2Position) override;
    void draw() const override;

    int getCount() const override { return 2; }
    void tryToCling(const LightString& string) override;
    void followString(Vector2 stringStart, Vector2 stringEnd) override;
    void letGo() override;

protected:
    Vector2 getCenter() const override;
    float getReach() const override;
    void moveCenterTo(Vector2 center) override;

private:
    // Put both bodies either side of center, along the link.
    void placeBodiesAround(Vector2 center);

    Vector2 partnerPosition;
    float partnerStringT; // the partner's spot on the string, from 0 to 1
    float linkAngle;      // direction of the link from the first body to the partner
};
