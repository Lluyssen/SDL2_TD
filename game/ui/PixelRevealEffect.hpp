#pragma once

#include <SDL2/SDL.h>
#include <vector>
#include <random>
#include <algorithm>

struct PixelBlock
{
    SDL_Rect src;
    SDL_Rect dst;
    float delay;
};

class PixelRevealEffect
{
private:
    std::vector<PixelBlock> blocks;

public:
    void generate(const SDL_Rect &src, const SDL_Rect &dst, int blockSize = 8)
    {
        blocks.clear();

        float scale = std::min(
            float(dst.w) / src.w,
            float(dst.h) / src.h);

        int drawW = std::round(src.w * scale);
        int drawH = std::round(src.h * scale);

        int offsetX = dst.x + (dst.w - drawW) / 2;
        int offsetY = dst.y + (dst.h - drawH) / 2;

        float centerX = src.w * 0.5f;
        float centerY = src.h * 0.5f;

        float maxDist = centerX * centerX + centerY * centerY;

        for (int y = 0; y < src.h; y += blockSize)
        {
            for (int x = 0; x < src.w; x += blockSize)
            {
                int bw = std::min(blockSize, src.w - x);
                int bh = std::min(blockSize, src.h - y);

                PixelBlock b;

                b.src = {src.x + x, src.y + y, bw, bh};

                int x1 = offsetX + std::round(x * scale);
                int y1 = offsetY + std::round(y * scale);
                int x2 = offsetX + std::round((x + bw) * scale);
                int y2 = offsetY + std::round((y + bh) * scale);

                b.dst =
                    {
                        x1,
                        y1,
                        x2 - x1,
                        y2 - y1};

                // distance au centre
                float dx = (x + bw * 0.5f) - centerX;
                float dy = (y + bh * 0.5f) - centerY;

                float dist = dx * dx + dy * dy;

                // délai normalisé
                b.delay = dist / maxDist;

                blocks.push_back(b);
            }
        }

        // léger mélange pour casser l'effet trop parfait
        std::shuffle(blocks.begin(), blocks.end(), std::mt19937{std::random_device{}()});
    }

    void render(SDL_Renderer *renderer, SDL_Texture *tex, float t)
    {
        for (const auto &b : blocks)
        {
            if (t >= b.delay)
                SDL_RenderCopy(renderer, tex, &b.src, &b.dst);
        }
    }

    void reset()
    {
        for (auto &b : blocks)
            b.delay = 0;
    }
};