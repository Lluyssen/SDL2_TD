#include "raylib.h"

#include "utils/StateManager.hpp"
#include "utils/GameContext.hpp"
#include "states/MenuState.hpp"
#include "states/SandBoxState.hpp"

const bool START_IN_SANDBOX = false;

int main(void)
{
    // Contexte global
    GameContext context;
    context.setResolution(1920, 1080);

    // Init raylib
    InitWindow(context.getWidth(), context.getHeight(), "Tower Defense");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();

    // Manager de states
    StateManager stateManager(context);

    // Premier état
    if (START_IN_SANDBOX)
        stateManager.pushState<SandBoxState>();
    else
        stateManager.pushState<MenuState>();

    // Boucle principale
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Update logique
        stateManager.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        // Render
        stateManager.render();

        EndDrawing();

        // Update musique (important pour raylib)
        context.updateMusic();
    }

    // Nettoyage propre
    context.unloadAllTextures();
    CloseAudioDevice();
    CloseWindow();

    return 0;
}