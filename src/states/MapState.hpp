#pragma once

#include "../utils/StateManager.hpp"
#include "PauseMenuState.hpp"


class MapState : public IGameState
{
private:

    Texture2D _background{};

public:

    void onEnter(StateManager &sm) override
    {
        _background = LoadTexture("../assets/ui/bg/Level.jpg");
    }

    void update(StateManager &sm, float dt) override
    {
        // ouvrir pause
        (void) dt;
        if (IsKeyPressed(KEY_ESCAPE))
        {
            sm.pushState<PauseMenuState>();
            return;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        ClearBackground(BLACK);

        Rectangle src{
            0,0,
            (float)_background.width,
            (float)_background.height
        };

        Rectangle dst{
            0,0,
            (float)w,
            (float)h
        };

        DrawTexturePro(_background,src,dst,{0,0},0,WHITE);
    }

    void onExit(StateManager &) override
    {
        UnloadTexture(_background);
    }
};