#pragma once
#include "raylib.h"
#include <vector>
#include <math.h>

struct Star
{
    Vector2 pos;
    Vector2 vel;
    float size;
    float phase;
    float speed;
};

class Starfield
{
private:
    std::vector<Star> _far;
    std::vector<Star> _mid;
    std::vector<Star> _near;

public:
    void init(int w, int h)
    {
        auto spawn = [&](std::vector<Star> &stars, int count, float sizeMin, float sizeMax)
        {
            for (int i = 0; i < count; i++)
            {
                stars.push_back({{(float)GetRandomValue(0, w),
                                  (float)GetRandomValue(0, h)},

                                 {(float)GetRandomValue(-20, 20),
                                  (float)GetRandomValue(-20, 20)},

                                 GetRandomValue(10, 30) / 10.0f,
                                 GetRandomValue(0, 628) / 100.0f});
            }
        };

        spawn(_far, 80, 1.0f, 2.0f);
        spawn(_mid, 40, 2.0f, 3.0f);
        spawn(_near, 20, 3.0f, 5.0f);
    }

    void update(float dt, int w, int h)
    {
        auto updateLayer = [&](std::vector<Star> &stars, float speed)
        {
            for (auto &s : stars)
            {
                s.phase += dt * 2.0f;

                s.pos.x += s.vel.x * dt;
                s.pos.y += s.vel.y * dt;

                if (s.pos.x < 0)
                    s.pos.x += w;
                if (s.pos.x > w)
                    s.pos.x -= w;

                if (s.pos.y < 0)
                    s.pos.y += h;
                if (s.pos.y > h)
                    s.pos.y -= h;
            }
        };

        updateLayer(_far, 10);
        updateLayer(_mid, 20);
        updateLayer(_near, 40);
    }

    void draw(int w, int h)
    {
        Vector2 mouse = GetMousePosition();

        float nx = (mouse.x / w) - 0.5f;
        float ny = (mouse.y / h) - 0.5f;

        auto drawLayer = [&](std::vector<Star> &stars, float strength)
        {
            for (auto &s : stars)
            {
                float twinkle = (sinf(s.phase * 3.0f) + 1.0f) * 0.5f;

                float brightness = 160 + twinkle * 80;

                Color c{
                    (unsigned char)brightness,
                    (unsigned char)brightness,
                    (unsigned char)(brightness + 20),
                    255};

                float size = s.size + twinkle * 0.4f;

                DrawCircleV(s.pos, size, c);
            }
        };

        drawLayer(_far, 5);
        drawLayer(_mid, 15);
        drawLayer(_near, 30);
    }
};