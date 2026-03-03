#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "../../engine/UISprites.hpp"
#include "GameState.hpp"
#include <SDL2/SDL_ttf.h>
#include <vector>

constexpr int TILE_SIZE = 32;

class MenuState : public IGameState
{
private:
    enum class ButtonType
    {
        None,
        Play,
        Options,
        Exit
    };

    struct Button
    {
        ButtonType type;
        const SpriteDef *sprite;
    };

    std::vector<Button> buttons;
    std::vector<SDL_Rect> cachedRects;

    ButtonType hovered = ButtonType::None;

    bool appeared = false;
    float appearTimer = 0.f;
    const float appearDuration = 0.25f;

    int hoverTick = 0;

    bool transitioning = false;
    Uint8 transitionAlpha = 0;

    SDL_Texture *uiTexture = nullptr;        // non-owning
    SDL_Texture *backgroundTarget = nullptr; // owned
    SDL_Texture *backgroundImage = nullptr;

    bool layoutDirty = true;

    std::vector<SDL_Rect> computeLayout(int w, int h) const
    {
        std::vector<SDL_Rect> r;

        int bw = w / 3;
        int bh = TILE_SIZE * 2;
        int spacing = TILE_SIZE;

        int totalH = buttons.size() * bh + (buttons.size() - 1) * spacing;
        int y = (h - totalH) / 2;
        int cx = w / 2;

        for (size_t i = 0; i < buttons.size(); ++i)
            r.push_back({cx - bw / 2, y + int(i) * (bh + spacing), bw, bh});
        return r;
    }

    ButtonType getButtonAt(int x, int y) const
    {
        for (size_t i = 0; i < cachedRects.size(); ++i)
        {
            const auto &r = cachedRects[i];
            if (x >= r.x && x <= r.x + r.w && y >= r.y && y <= r.y + r.h)
                return buttons[i].type;
        }
        return ButtonType::None;
    }

public:
    virtual ~MenuState(void) = default;

    MenuState()
    {
        buttons = {
            {ButtonType::Play, &UISprites::Play},
            {ButtonType::Options, &UISprites::Options},
            {ButtonType::Exit, &UISprites::Exit}};
    }

    void onEnter(StateManager &sm) override
    {
        SDL_ShowCursor(SDL_ENABLE);

        appearTimer = 0.f;
        appeared = false;
        hoverTick = 0;
        transitioning = false;
        transitionAlpha = 0;
        layoutDirty = true;

        auto *renderer = sm.getContext().renderer;
        auto *assets = sm.getContext().assets;

        // UI sprite sheet
        assets->loadTexture("ui", "assets/ui/spriteMenu.png");
        uiTexture = assets->getTexture("ui");

        assets->loadTexture("menu_bg", "assets/ui/menu_bg.png");
        backgroundImage = assets->getTexture("menu_bg");

        // Background render target (si tu veux garder le noise)
        int w;
        int h;
        SDL_GetRendererOutputSize(renderer, &w, &h);
        backgroundTarget = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
    }

    void onExit(StateManager &) override
    {
        if (backgroundTarget)
        {
            SDL_DestroyTexture(backgroundTarget);
            backgroundTarget = nullptr;
        }
    }

    void handleEvent(StateManager &, const SDL_Event &e) override
    {
        if (transitioning)
            return;

        if (e.type == SDL_WINDOWEVENT && e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            layoutDirty = true;

        if (e.type == SDL_MOUSEMOTION)
            hovered = getButtonAt(e.motion.x, e.motion.y);

        if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT)
        {
            if (hovered == ButtonType::Play)
            {
                transitioning = true;
                transitionAlpha = 0;
            }
            else if (hovered == ButtonType::Exit)
                SDL_Log("QUIT GAME");
        }
    }

    void update(StateManager &sm, float dt) override
    {
        if (!transitioning)
        {
            if (!appeared) // Apparition une fois sinon il disparait
            {
                appearTimer += dt;
                if (appearTimer >= appearDuration)
                {
                    appearTimer = appearDuration;
                    appeared = true;
                }
            }

            hoverTick = (hoverTick + 1) % 30;
        }
        else
        {
            // Transition fade sur click
            transitionAlpha = std::min<Uint8>(255, transitionAlpha + static_cast<Uint8>(dt * 255.f * 3.f));

            if (transitionAlpha > 250)
                sm.changeState<GameState>();
        }
    }

    void render(StateManager &sm) override
    {
        SDL_Renderer *renderer = sm.getContext().renderer;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        if (layoutDirty)
        {
            cachedRects = computeLayout(w, h);
            layoutDirty = false;
        }

        if (backgroundImage)
        {
            SDL_Rect dst{0, 0, w, h};
            SDL_RenderCopy(renderer, backgroundImage, nullptr, &dst);
        }

        float t = appearTimer / appearDuration;
        if (t > 1.f)
            t = 1.f;

        int step = static_cast<int>(t * 8.f); // 0 → 8
        int slide = (8 - step) * 4;           // 32 px max
        Uint8 bgAlpha = static_cast<Uint8>(step * 32);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (size_t i = 0; i < cachedRects.size(); ++i)
        {
            SDL_Rect dst = cachedRects[i];
            dst.y += slide;

            const auto &btn = buttons[i];
            bool isHover = (hovered == btn.type);
            bool pulse = (hoverTick < 15);

            Uint8 shade = (isHover && pulse) ? 200 : 140;
            SDL_SetRenderDrawColor(renderer, shade, shade, shade + 20, bgAlpha);
            SDL_RenderFillRect(renderer, &dst);

            SDL_SetTextureAlphaMod(uiTexture, 255);
            SDL_RenderCopy(renderer, uiTexture, &btn.sprite->src, &dst);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        if (transitioning)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, transitionAlpha);
            SDL_Rect full{0, 0, w, h};
            SDL_RenderFillRect(renderer, &full);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }
};