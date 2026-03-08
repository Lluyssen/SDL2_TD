#include "raylib.h"

#include "utils/StateManager.hpp"
#include "utils/GameContext.hpp"
#include "states/MenuState.hpp"

int main(void)
{
    // Initialisation raylib
    GameContext context;

    context.setResolution(1280, 720);

    InitWindow(context.getWidth(), context.getHeight(), "Tower Defense");
    SetTargetFPS(60);
    InitAudioDevice();

    // Contexte global du jeu
    
    // Manager de states
    StateManager stateManager(context);

    // Premier état
    stateManager.pushState<MenuState>();

    // Boucle principale
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();

        // Update logique
        stateManager.update(dt);

        // Render
        BeginDrawing();
        ClearBackground(BLACK);

        stateManager.render();

        EndDrawing();
    }

    CloseWindow();

    return 0;
}