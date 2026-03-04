#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "../../engine/UISprites.hpp"
#include "GameState.hpp"
#include "../Menu.hpp"

#include "../ui/AnimatedBackGround.hpp"
#include "../ui/ScreenFade.hpp"
#include "../ui/AmbientParticles.hpp"
#include "../ui/MenuTitle.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <vector>
#include <string>
#include <algorithm>

class MenuState : public IGameState
{
private:
    Menu menu;

    AnimatedBackground background;
    AmbientParticles particles;
    ScreenFade fade;
    MenuTitle title;

    SDL_Texture *uiTexture = nullptr;
    SDL_Texture *titleTexture = nullptr;

    Mix_Music *music = nullptr;
    Mix_Chunk *clickSound = nullptr;

    float appearTimer = 0.f;
    const float appearDuration = 1.f;

    float parallaxX = 0.f;
    float parallaxY = 0.f;

public:
    void onEnter(StateManager &sm) override
    {
        auto *renderer = sm.getContext().renderer;
        auto *assets = sm.getContext().assets;

        SDL_ShowCursor(SDL_ENABLE);

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        particles.init(120, w, h);
        title.setPosition(w);

        appearTimer = 0.f;

        uiTexture = assets->getTexture("ui");
        if (!uiTexture)
        {
            assets->loadTexture("ui", "assets/ui/spriteMenu.png");
            uiTexture = assets->getTexture("ui");
        }

        titleTexture = assets->getTexture("title");
        if (!titleTexture)
        {
            assets->loadTexture("title", "assets/ui/title.png");
            titleTexture = assets->getTexture("title");
        }

        clickSound = Mix_LoadWAV("assets/audio/click.wav");

        if (!music)
        {
            music = Mix_LoadMUS("assets/audio/menu_music.mp3");
            Mix_PlayMusic(music, -1);
        }

        buildMenu(sm);
        buildBackground(renderer);
    }

    void buildMenu(StateManager &sm)
    {
        menu.build(
            {{UISprites::Play.src,
              [this, &sm]
              {
                  playClick();
                  fade.start();
                  sm.changeState<GameState>();
              }},

             {UISprites::Options.src,
              [this]
              {
                  playClick();
              }},

             {UISprites::Exit.src,
              [this]
              {
                  playClick();
                  SDL_Event e;
                  e.type = SDL_QUIT;
                  SDL_PushEvent(&e);
              }}});
    }

    void playClick()
    {
        if (clickSound)
            Mix_PlayChannel(-1, clickSound, 0);
    }

    void buildBackground(SDL_Renderer *renderer)
    {
        background.addLayer(0.2f);
        background.addLayer(0.5f);
        background.addLayer(1.0f);

        for (int i = 0;; i++)
        {
            std::string file =
                "assets/ui/bg/frame" +
                std::to_string(i) +
                ".png";

            SDL_Surface *surf = IMG_Load(file.c_str());

            if (!surf)
                break;

            SDL_Texture *tex =
                SDL_CreateTextureFromSurface(renderer, surf);

            SDL_FreeSurface(surf);

            background.addFrame(0, tex);
            background.addFrame(1, tex);
            background.addFrame(2, tex);
        }

        background.reset();
    }

    void handleEvent(StateManager &sm, const SDL_Event &e) override
    {
        if (fade.isActive())
            return;

        menu.handleEvent(e);

        if (e.type == SDL_MOUSEMOTION)
        {
            int w, h;
            SDL_GetRendererOutputSize(
                sm.getContext().renderer, &w, &h);

            float nx = (float)e.motion.x / w - 0.5f;
            float ny = (float)e.motion.y / h - 0.5f;

            parallaxX = nx * 20.f;
            parallaxY = ny * 20.f;
        }
    }

    void update(StateManager &sm, float dt) override
    {
        appearTimer =
            std::min(appearTimer + dt, appearDuration);

        int w, h;
        SDL_GetRendererOutputSize(
            sm.getContext().renderer, &w, &h);

        particles.update(dt, w, h);
        background.update(dt);
        fade.update(dt);
    }

    void render(StateManager &sm) override
    {
        auto *renderer = sm.getContext().renderer;

        int w, h;
        SDL_GetRendererOutputSize(renderer, &w, &h);

        background.render(renderer, w, h, parallaxX, parallaxY);
        title.render(renderer, titleTexture);
        particles.render(renderer);

        float appear = std::clamp(
            appearTimer / appearDuration,
            0.f,
            1.f);

        menu.render(renderer, uiTexture, appear);

        fade.render(renderer, w, h);
    }

    bool allowUpdateBelow() const override { return false; }
    bool allowRenderBelow() const override { return false; }
};