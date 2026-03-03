#pragma once

#include "../../engine/State.hpp"
#include "../../engine/StateManager.hpp"
#include "GameState.hpp"

class GameState;

class PauseState : public IGameState
{
public:
    PauseState(void) = default;
    virtual ~PauseState(void) = default;
    bool allowUpdateBelow() const override { return false; }
    bool allowRenderBelow() const override { return true; }

    void handleEvent(StateManager &sm, const SDL_Event &e) override
    {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE)
            sm.popState();
    }

    void update(StateManager &, float) override {}

    void render(StateManager &sm) override
    {
        auto *renderer = sm.getContext().renderer;

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);

        SDL_Rect overlay{0, 0, 800, 600};
        SDL_RenderFillRect(renderer, &overlay);

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
    }
};