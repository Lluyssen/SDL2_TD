#pragma once

#include "PixelRevealEffect.hpp"
#include <functional>

class UIButton
{
private:
    SDL_Rect rect{};
    SDL_Rect src{};
    SDL_Rect visualRect{};

    PixelRevealEffect reveal;

    bool hovered = false;

public:
    std::function<void()> onClick;

    void setSprite(const SDL_Rect &s)
    {
        src = s;
    }

    void setRect(const SDL_Rect &r)
    {
        rect = r;

        float scale = std::min(
            float(rect.w) / src.w,
            float(rect.h) / src.h);

        int drawW = std::round(src.w * scale);
        int drawH = std::round(src.h * scale);

        visualRect =
            {
                rect.x + (rect.w - drawW) / 2,
                rect.y + (rect.h - drawH) / 2,
                drawW,
                drawH};

        reveal.generate(src, rect);
    }

    bool contains(int x, int y) const
    {
        return x >= rect.x &&
               x <= rect.x + rect.w &&
               y >= rect.y &&
               y <= rect.y + rect.h;
    }

    void setHovered(bool h)
    {
        hovered = h;
    }

    void click()
    {
        if (onClick)
            onClick();
    }

    void render(SDL_Renderer *renderer, SDL_Texture *texture, float t)
    {
        reveal.render(renderer, texture, t);

        if (hovered)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 40);
            SDL_RenderFillRect(renderer, &visualRect);
        }
    }

    void resetReveal()
    {
        reveal.generate(src, rect);
    }
};