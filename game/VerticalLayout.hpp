#pragma once

#include <vector>
#include <SDL2/SDL.h>

class VerticalLayout
{
public:

    static std::vector<SDL_Rect> build(
        int screenW,
        int screenH,
        int count,
        int itemW,
        int itemH,
        int spacing)
    {
        std::vector<SDL_Rect> rects;

        int totalH = count * itemH + (count - 1) * spacing;
        int startY = (screenH - totalH) / 2;
        int cx = screenW / 2;

        for (int i = 0; i < count; i++)
        {
            rects.push_back({
                cx - itemW / 2,
                startY + i * (itemH + spacing),
                itemW,
                itemH
            });
        }

        return rects;
    }
};