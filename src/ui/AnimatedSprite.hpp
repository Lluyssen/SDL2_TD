#pragma once

#include "raylib.h"
#include <vector>
#include <string>

class GameContext;

class AnimatedSprite
{
private:
    std::vector<Texture2D *> _frames;

    int _currentFrame = 0;
    float _timer = 0.f;
    float _frameTime = 0.05f;

public:
    AnimatedSprite() = default;

    void load(GameContext &ctx, const std::string &basePath, int frameCount, float frameTime = 0.05f)
    {
        _frames.clear();
        _frameTime = frameTime;

        for (int i = 0; i < frameCount; i++)
        {
            std::string path = basePath + std::to_string(i) + ".png";
            Texture2D &tex = ctx.loadTexture(path);
            _frames.push_back(&tex);
        }
    }

    void update(float dt)
    {
        _timer += dt;
        if (_timer >= _frameTime)
        {
            _currentFrame++;
            if (_currentFrame >= (int)_frames.size())
                _currentFrame = 0;
            _timer = 0.f;
        }
    }

    void drawFullscreen(int width, int height)
    {
        if (_frames.empty())
            return;

        Texture2D &tex = *_frames[_currentFrame];
        DrawTexturePro(
            tex,
            {0, 0, (float)tex.width, (float)tex.height},
            {0, 0, (float)width, (float)height},
            {0, 0},
            0,
            WHITE);
    }

    void drawFullscreen(int w, int h, float offsetX, float offsetY)
    {
        Texture2D &tex = *_frames[_currentFrame];

        float zoom = 1.1f; // 10% plus grand que l'écran

        float drawW = w * zoom;
        float drawH = h * zoom;

        Rectangle src{0, 0, (float)tex.width, (float)tex.height};

        Rectangle dst{
            (w - drawW) * 0.5f + offsetX,
            (h - drawH) * 0.5f + offsetY,
            drawW,
            drawH};

        DrawTexturePro(tex, src, dst, {0, 0}, 0.0f, WHITE);
    }
};