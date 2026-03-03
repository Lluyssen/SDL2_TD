#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "../../engine/UISprites.hpp"
#include "GameState.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <vector>
#include <map>
#include <random>

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
        std::function<void()> onClick;
    };

    struct PixelBlock
    {
        SDL_Rect src;
        SDL_Rect dst;
        float revealTime;
    };

    std::vector<Button> buttons;
    std::vector<SDL_Rect> cachedRects;
    ButtonType hovered = ButtonType::None;

    // Animation menu
    bool appeared = false;
    float appearTimer = 0.f;
    const float appearDuration = 1.0f;
    float hoverTime = 0.f;

    // Transition
    bool transitioning = false;
    float transitionAlpha = 0.f;
    const float transitionDuration = 0.5f;

    SDL_Texture *uiTexture = nullptr;

    // Fond animé
    std::vector<SDL_Texture *> backgroundFrames;
    float backgroundTime = 0.f;
    size_t currentBackgroundFrame = 0;
    const float frameDuration = 0.3f;

    // Musique
    Mix_Music *bgMusic = nullptr;

    bool layoutDirty = true;

    // Blocs pixel par bouton
    std::vector<std::vector<PixelBlock>> buttonBlocks;

    // Layout
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

    void updateLayoutIfNeeded(SDL_Renderer *renderer)
    {
        if (layoutDirty)
        {
            int w, h;
            SDL_GetRendererOutputSize(renderer, &w, &h);
            cachedRects = computeLayout(w, h);
            layoutDirty = false;
        }
    }

    // Prépare les blocs pixel aléatoires depuis le centre du bouton
    std::vector<PixelBlock> preparePixelBlocks(const Button &btn, const SDL_Rect &dst, int blockSize = 4)
    {
        std::vector<PixelBlock> blocks;
        SDL_Rect src = btn.sprite->src;
        int centerX = src.w / 2;
        int centerY = src.h / 2;
        int offsetX = dst.w / 2 - src.w / 2;
        int offsetY = dst.h / 2 - src.h / 2;

        std::map<int, std::vector<PixelBlock>> rings;

        for (int y = 0; y < src.h; y += blockSize)
        {
            for (int x = 0; x < src.w; x += blockSize)
            {
                float dx = (x + blockSize / 2) - centerX;
                float dy = (y + blockSize / 2) - centerY;
                float distance = sqrtf(dx * dx + dy * dy);

                PixelBlock b;
                b.src = {x + src.x, y + src.y, blockSize, blockSize};
                b.dst = {dst.x + x + offsetX, dst.y + y + offsetY, blockSize, blockSize};
                b.revealTime = 0.f;

                int ring = static_cast<int>(distance / blockSize);
                rings[ring].push_back(b);
            }
        }

        for (auto &[ring, vec] : rings)
        {
            std::shuffle(vec.begin(), vec.end(), std::mt19937{std::random_device{}()});
            for (auto &b : vec)
            {
                b.revealTime = static_cast<float>(ring) / rings.size();
                blocks.push_back(b);
            }
        }

        return blocks;
    }

    void drawButtonRandomBlocks(SDL_Renderer *renderer, const std::vector<PixelBlock> &blocks, float appearT)
    {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        for (const auto &b : blocks)
        {
            if (appearT >= b.revealTime)
            {
                Uint8 alphaShadow = 25;
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, alphaShadow);
                SDL_RenderFillRect(renderer, &b.dst);
                SDL_SetTextureAlphaMod(uiTexture, 255);
                SDL_RenderCopy(renderer, uiTexture, &b.src, &b.dst);
            }
        }
    }

public:
    MenuState()
    {
        buttons = {
            {ButtonType::Play, &UISprites::Play, nullptr},
            {ButtonType::Options, &UISprites::Options, nullptr},
            {ButtonType::Exit, &UISprites::Exit, nullptr}};
    }

    ~MenuState()
    {
        for (auto *tex : backgroundFrames)
            SDL_DestroyTexture(tex);
        if (bgMusic)
        {
            Mix_HaltMusic();
            Mix_FreeMusic(bgMusic);
        }
    }

    void onEnter(StateManager &sm) override
    {
        SDL_ShowCursor(SDL_ENABLE);

        appearTimer = 0.f;
        appeared = false;
        hoverTime = 0.f;
        transitioning = false;
        transitionAlpha = 0.f;
        layoutDirty = true;

        auto *assets = sm.getContext().assets;
        SDL_Renderer *renderer = sm.getContext().renderer;

        // UI
        if (!uiTexture)
        {
            assets->loadTexture("ui", "assets/ui/spriteMenu.png");
            uiTexture = assets->getTexture("ui");
        }

        // Fond animé : recréation complète
        for (auto *tex : backgroundFrames)
            SDL_DestroyTexture(tex);
        backgroundFrames.clear();

        for (int i = 0;; ++i)
        {
            std::string file = "assets/ui/bg/frame" + std::to_string(i) + ".png";
            SDL_Surface *surf = IMG_Load(file.c_str());
            if (!surf)
                break;
            SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_FreeSurface(surf);
            if (!tex)
                break;
            backgroundFrames.push_back(tex);
        }

        currentBackgroundFrame = 0;
        backgroundTime = 0.f;

        // Musique
        if (!bgMusic)
        {
            bgMusic = Mix_LoadMUS("assets/audio/menu_music.mp3");
            if (bgMusic)
                Mix_PlayMusic(bgMusic, -1);
        }

        // Layout et blocs pixel
        updateLayoutIfNeeded(renderer);
        buttonBlocks.clear();
        for (size_t i = 0; i < buttons.size(); ++i)
            buttonBlocks.push_back(preparePixelBlocks(buttons[i], cachedRects[i], 4));

        // Callbacks
        for (auto &btn : buttons)
        {
            if (btn.type == ButtonType::Play)
                btn.onClick = [&sm]()
                { sm.changeState<GameState>(); };
            else if (btn.type == ButtonType::Exit)
                btn.onClick = []()
                { SDL_Log("QUIT GAME"); };
        }
    }

    void onExit(StateManager &) override
    {
        if (bgMusic)
            Mix_HaltMusic();
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
            for (auto &btn : buttons)
            {
                if (hovered == btn.type && btn.onClick)
                {
                    transitioning = true;
                    transitionAlpha = 0.f;
                    break;
                }
            }
        }
    }

    void update(StateManager &, float dt) override
    {
        if (!transitioning)
        {
            if (!appeared)
            {
                appearTimer += dt;
                if (appearTimer >= appearDuration)
                {
                    appearTimer = appearDuration;
                    appeared = true;
                }
            }
            hoverTime += dt;
        }

        // Animation fond
        backgroundTime += dt;
        if (!backgroundFrames.empty() && backgroundTime >= frameDuration)
        {
            backgroundTime -= frameDuration;
            currentBackgroundFrame = (currentBackgroundFrame + 1) % backgroundFrames.size();
        }

        // Transition fade
        if (transitioning)
        {
            transitionAlpha += (dt / transitionDuration) * 255.f;
            if (transitionAlpha >= 255.f)
            {
                for (auto &btn : buttons)
                {
                    if (btn.type == ButtonType::Play && btn.onClick)
                        btn.onClick();
                }
            }
        }
    }

    void render(StateManager &sm) override
    {
        SDL_Renderer *renderer = sm.getContext().renderer;
        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        updateLayoutIfNeeded(renderer);

        // Fond animé
        if (!backgroundFrames.empty())
        {
            SDL_Rect dst{0, 0, w, h};
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
            SDL_RenderCopy(renderer, backgroundFrames[currentBackgroundFrame], nullptr, &dst);
        }

        float appearT = std::clamp(appearTimer / appearDuration, 0.f, 1.f);

        for (size_t i = 0; i < buttons.size(); ++i)
            drawButtonRandomBlocks(renderer, buttonBlocks[i], appearT);

        if (transitioning)
        {
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, static_cast<Uint8>(transitionAlpha));
            SDL_Rect full{0, 0, w, h};
            SDL_RenderFillRect(renderer, &full);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        }
    }

    bool allowUpdateBelow() const override { return false; }
    bool allowRenderBelow() const override { return false; }
};