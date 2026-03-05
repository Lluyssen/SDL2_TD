#pragma once

#include "UIAnimation.hpp"
#include "../UIButton.hpp"
#include <math.h>
#include "raymath.h"

class PixelRevealAnimation : public UIAnimation
{
private:
    float _timer = 0.0f;
    float _duration = 0.8f;

    int _blockSize = 12;

public:
    void reset() override
    {
        _timer = 0.0f;
    }

    void update(UIButton &btn, float dt) override
    {
        _timer += dt;
    }

    bool insideRounded(Rectangle r, float x, float y, float radius)
    {
        float left = r.x + radius;
        float right = r.x + r.width - radius;
        float top = r.y + radius;
        float bottom = r.y + r.height - radius;

        if (x >= left && x <= right)
            return true;

        if (y >= top && y <= bottom)
            return true;

        float dx = (x < left) ? left - x : x - right;
        float dy = (y < top) ? top - y : y - bottom;

        return (dx * dx + dy * dy) <= radius * radius;
    }

    void draw(UIButton &btn) override
    {
        Rectangle r = btn.rect();

        float progress = _timer / _duration;

        int cols = r.width / _blockSize;
        int rows = r.height / _blockSize;

        Vector2 center =
            {
                r.x + r.width * 0.5f,
                r.y + r.height * 0.5f};

        float maxDist =
            sqrtf((r.width * 0.5f) * (r.width * 0.5f) +
                  (r.height * 0.5f) * (r.height * 0.5f));

        float radius = r.height * 0.3f;

        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                float px = r.x + x * _blockSize;
                float py = r.y + y * _blockSize;

                float cx = px + _blockSize * 0.5f;
                float cy = py + _blockSize * 0.5f;

                float dx = cx - center.x;
                float dy = cy - center.y;

                float dist = (dx * dx + dy * dy) / (maxDist * maxDist);

                float noise = fmodf(sinf(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0f);

                if (dist + noise * 0.25f <= progress &&
                    insideRounded(r, cx, cy, radius))
                {
                    DrawRectangle(px, py, _blockSize, _blockSize, Color{0, 0, 0, 120});
                }
            }
        }

        // texte visible pendant l'animation
        int fs = 28;
        int tw = MeasureText(btn.text().c_str(), fs);

        DrawText(
            btn.text().c_str(),
            r.x + (r.width - tw) / 2,
            r.y + (r.height - fs) / 2,
            fs,
            btn.hover() ? YELLOW : WHITE);

        if (progress >= 1.0f)
            btn.finishEnter();
    }
};