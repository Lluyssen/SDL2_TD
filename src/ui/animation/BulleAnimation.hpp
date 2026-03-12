#pragma once

#include "raylib.h"
#include <vector>
#include <algorithm>
#include <cmath>

struct Bubble
{
    Vector2 pos{};
    float radius{};
    float speed{};
    float life{};
    float maxLife{};
    float phase{};
};

class BubbleAnimation
{
private:
    std::vector<Bubble> _bubbles;
    Vector2 _pos;

public:
    void reserve(size_t n)
    {
        _bubbles.reserve(n);
    }

    void clear()
    {
        _bubbles.clear();
    }

    void setPos(Vector2 pos) { _pos = pos; }
    Vector2 getPos(void) { return _pos; }

    void spawn(void)
    {
        Bubble b;

        b.pos = _pos;

        b.pos.x += GetRandomValue(-20, 20);
        b.pos.y += GetRandomValue(-10, 10);

        b.radius = (float)GetRandomValue(3, 8);
        b.speed = (float)GetRandomValue(15, 40);

        b.life = 0.f;
        b.maxLife = (float)GetRandomValue(2, 4);

        b.phase = (float)GetRandomValue(0, 628) / 100.f;

        _bubbles.push_back(b);
    }

    void update(float dt)
    {
        for (auto &b : _bubbles)
        {
            b.life += dt;

            b.pos.y -= b.speed * dt;

            // oscillation horizontale
            b.pos.x += std::sin(b.life * 3.f + b.phase) * 20.f * dt;
        }

        _bubbles.erase(
            std::remove_if(_bubbles.begin(), _bubbles.end(),
                           [](const Bubble &b)
                           {
                               return b.life >= b.maxLife;
                           }),
            _bubbles.end());
    }

    void draw(Color core = GREEN, Color glow = LIME)
    {
        for (const auto &b : _bubbles)
        {
            float alpha = 1.f - (b.life / b.maxLife);
            DrawCircleV(b.pos, b.radius * 2.f, Fade(glow, alpha * 0.2f));
            DrawCircleV(b.pos, b.radius, Fade(core, alpha));
            DrawCircleLines(b.pos.x, b.pos.y, b.radius, Fade(glow, alpha));
        }
    }
};