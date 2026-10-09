#include "Particles.h"

#include <cmath>

#include "raymath.h"

namespace {
const float MIN_SPEED = 40.0f;     // pixels per second when thrown out
const float MAX_SPEED = 170.0f;
const float MIN_LIFETIME = 0.5f;   // seconds
const float MAX_LIFETIME = 1.2f;
const float DRAG = 2.5f;           // how quickly sparks slow down; higher stops sooner
const float START_SIZE = 3.0f;     // radius when new; shrinks to nothing
const Color SPARK_COLORS[] = {
    {255, 220, 140, 255}, // warm yellow, like the fireflies
    {255, 170, 80, 255},  // orange, like the lantern
    {255, 240, 200, 255}, // nearly white
};

float randomFloat(float min, float max)
{
    return min + (max - min) * GetRandomValue(0, 10000) / 10000.0f;
}
}

void Particles::burst(Vector2 at, int count)
{
    int colorCount = sizeof(SPARK_COLORS) / sizeof(SPARK_COLORS[0]);
    for (int i = 0; i < count; i++) {
        float angle = randomFloat(0.0f, 2.0f * PI);
        float speed = randomFloat(MIN_SPEED, MAX_SPEED);
        float lifetime = randomFloat(MIN_LIFETIME, MAX_LIFETIME);
        Spark spark;
        spark.position = at;
        spark.velocity = {std::cos(angle) * speed, std::sin(angle) * speed};
        spark.life = lifetime;
        spark.lifetime = lifetime;
        spark.color = SPARK_COLORS[GetRandomValue(0, colorCount - 1)];
        sparks.push_back(spark);
    }
}

void Particles::update(float dt)
{
    // Losing the same fraction of speed every second, whatever the frame rate.
    float keep = std::exp(-DRAG * dt);
    for (Spark& spark : sparks) {
        spark.position = Vector2Add(spark.position, Vector2Scale(spark.velocity, dt));
        spark.velocity = Vector2Scale(spark.velocity, keep);
        spark.life -= dt;
    }

    // Remove dead sparks. Only step forward when keeping one, since erasing
    // shifts the next spark into slot i.
    for (size_t i = 0; i < sparks.size();) {
        if (sparks[i].life <= 0.0f) {
            sparks.erase(sparks.begin() + i);
        } else {
            i++;
        }
    }
}

void Particles::draw() const
{
    for (const Spark& spark : sparks) {
        float remaining = spark.life / spark.lifetime; // 1 when new, 0 when gone
        DrawCircleV(spark.position, START_SIZE * remaining, Fade(spark.color, remaining));
    }
}
