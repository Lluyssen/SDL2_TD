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
    bool revealFinished = false;
    float scale = 1.0f;

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

    // Animation Button pixel -> stabilisation image fixe -> animation breath
    void render(SDL_Renderer *renderer, SDL_Texture *texture, float t)
    {
        float time = SDL_GetTicks() * 0.002f;
        float breathe = hovered ? 0.f : sin(time) * 0.03f;
        float targetScale = hovered ? 1.08f : 1.f;

        scale += (targetScale - scale) * 0.15f;

        float finalScale = scale + breathe;

        SDL_Rect scaled = visualRect;

        scaled.w *= finalScale;
        scaled.h *= finalScale;
        scaled.x -= (scaled.w - visualRect.w) / 2;
        scaled.y -= (scaled.h - visualRect.h) / 2;

        if (!revealFinished)
        {
            reveal.render(renderer, texture, t);

            if (t >= 1.0f)
                revealFinished = true;
        }
        else
            SDL_RenderCopy(renderer, texture, &src, &scaled);

        if (hovered)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 40);
            SDL_RenderFillRect(renderer, &scaled);
        }
    }

    void resetReveal()
    {
        reveal.generate(src, rect);
        revealFinished = false;
    }
};