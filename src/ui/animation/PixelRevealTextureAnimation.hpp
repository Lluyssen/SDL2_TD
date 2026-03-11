#pragma once

#include "raylib.h"
#include "UITextureAnimation.hpp"
#include <math.h>

class PixelRevealTextureAnimation : public UITextureAnimation
{
private:
    float _timer = 0.f;
    float _duration = 0.35f;
    int _block = 12;

public:
    void reset(void) override
    {
        _timer = 0.f;
    }

    void update(float dt) override
    {
        _timer += dt;
    }

    bool finished(void) const override
    {
        return _timer >= _duration;
    }

    void draw(Texture2D &tex, Rectangle src, Rectangle dst) override
    {
        float progress = std::min(_timer / _duration, 1.f);

        int cols = tex.width / _block;
        int rows = tex.height / _block;

        float scaleX = dst.width / tex.width;
        float scaleY = dst.height / tex.height;

        for (int y = 0; y < rows; y++)
        {
            for (int x = 0; x < cols; x++)
            {
                float noise = fmodf(sinf(x * 12.9898f + y * 78.233f) * 43758.5453f, 1.0f);
                if (noise <= progress)
                {
                    Rectangle srcBlock{ (float)x * _block, (float)y * _block, (float)_block, (float)_block};
                    Rectangle dstBlock{ dst.x + (float)x * _block * scaleX, dst.y + (float)y * _block * scaleY, _block * scaleX, _block * scaleY};
                    DrawTexturePro(tex, srcBlock, dstBlock, {0, 0}, 0, WHITE);
                }
            }
        }
    }
};