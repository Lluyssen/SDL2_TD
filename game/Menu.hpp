#pragma once

#include "VerticalLayout.hpp"
#include "ui/UIButton.hpp"

struct MenuItem
{
    SDL_Rect sprite;
    std::function<void()> action;
};

class Menu
{
private:

    std::vector<UIButton> buttons;
    bool layoutDirty = true;
    int TILE_SIZE = 32;

public:

    void build(const std::vector<MenuItem>& items)
    {
        buttons.clear();
        buttons.resize(items.size());

        for (size_t i = 0; i < items.size(); i++)
        {
            buttons[i].setSprite(items[i].sprite);
            buttons[i].onClick = items[i].action;
        }

        layoutDirty = true;
    }

    void rebuild(SDL_Renderer* renderer)
    {
        if (!layoutDirty)
            return;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        int bw = w / 3;
        int bh = TILE_SIZE * 2;

        auto rects = VerticalLayout::build(
            w,
            h,
            buttons.size(),
            bw,
            bh,
            TILE_SIZE);

        for (size_t i = 0; i < buttons.size(); i++)
            buttons[i].setRect(rects[i]);

        layoutDirty = false;
    }

    void handleEvent(const SDL_Event& e)
    {
        if (e.type == SDL_MOUSEMOTION)
        {
            int x = e.motion.x;
            int y = e.motion.y;

            for (auto& b : buttons)
                b.setHovered(b.contains(x, y));
        }

        if (e.type == SDL_MOUSEBUTTONDOWN &&
            e.button.button == SDL_BUTTON_LEFT)
        {
            int x = e.button.x;
            int y = e.button.y;

            for (auto& b : buttons)
            {
                if (b.contains(x, y))
                {
                    b.click();
                    break;
                }
            }
        }

        if (e.type == SDL_WINDOWEVENT &&
            e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
        {
            layoutDirty = true;
        }
    }

    void render(SDL_Renderer* renderer, SDL_Texture* texture, float appear)
    {
        rebuild(renderer);

        for (auto& b : buttons)
            b.render(renderer, texture, appear);
    }

};