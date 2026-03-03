#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include "../ecs/ECS.hpp"
#include "../game/Components.hpp"
#include "../game/Systems.hpp"
#include "StateManager.hpp"
#include "../game/states/MenuState.hpp"

class Game
{

private:
    SDL_Window *window = nullptr;
    SDL_Renderer *renderer = nullptr;
    bool running = false;
    Uint32 previousTime = 0;

    GameContext context;
    StateManager stateManager{context};

    void handleEvents()
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = false;
            if (e.type == SDL_KEYDOWN) // touche pressée
            {
                switch (e.key.keysym.sym)
                {
                case SDLK_UP:
                    std::cout << "Flèche Haut pressée => Menu\n";
                    stateManager.changeState<MenuState>();
                    break;
                case SDLK_DOWN:
                    std::cout << "Flèche Bas pressée => Game\n";
                    stateManager.changeState<GameState>();
                    break;
                }
            }
            stateManager.handleEvent(e);
        }
    }

    void update(float dt)
    {
        stateManager.update(dt);
    }

    void render()
    {
        SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
        SDL_RenderClear(renderer);
        stateManager.render();
        SDL_RenderPresent(renderer);
    }

public:
    Game(void) = default;
    virtual ~Game(void) = default;

    bool init(int width = 800, int height = 600)
    {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0)
        {
            std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
            return false;
        }

        IMG_Init(IMG_INIT_PNG);

        window = SDL_CreateWindow(
            "Tower Defence",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            width,
            height,
            SDL_WINDOW_SHOWN);

        if (!window)
        {
            std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
            return false;
        }

        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        if (!renderer)
        {
            std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
            return false;
        }
        context.window = window;
        context.renderer = renderer;
        context.assets = new AssetManager(renderer);

        stateManager.changeState<MenuState>();

        running = true;
        previousTime = SDL_GetTicks();
        return true;
    }

    void run()
    {
        const float fixedDelta = 1.0f / 60.0f;
        float accumulator = 0.f;

        while (running)
        {
            Uint32 current = SDL_GetTicks();
            float frameTime = (current - previousTime) / 1000.f;
            previousTime = current;
            accumulator += frameTime;

            handleEvents();

            while (accumulator >= fixedDelta)
            {
                update(fixedDelta);
                accumulator -= fixedDelta;
            }

            render();
        }
    }

    void clean()
    {
        delete context.assets;
        context.assets = nullptr;

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        IMG_Quit();
        SDL_Quit();
    }
};