#pragma once

#include <core/StateManager.hpp>
#include <states/PauseMenuState.hpp>


class SandBoxState : public IGameState
{
public:

    void onEnter(StateManager &sm) override
    {
    }

    void update(StateManager &sm, float dt) override
    {
        (void) dt;
        // Ouvrir pause avec Échap
        if (IsKeyPressed(KEY_ESCAPE))
            sm.pushState<PauseMenuState>();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Fond gris simple
        ClearBackground(GRAY);

        // Tu peux dessiner des éléments de debug ici si besoin
        DrawText("Sandbox State", w/2 - 60, h/2, 20, WHITE);
    }

    void onExit(StateManager &) override
    {
        // Rien à décharger
    }
};