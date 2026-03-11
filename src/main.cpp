#include "raylib.h"

#include "core/StateManager.hpp"
#include "core/GameContext.hpp"
#include "states/MenuState.hpp"
#include "states/SandBoxState.hpp"
#include <states/LevelSelectionState.hpp>
#include <iostream>

const bool START_IN_SANDBOX = false;

int main(void)
{
    // Contexte global
    GameContext context;

    SetConfigFlags(FLAG_WINDOW_HIGHDPI);
    context.setResolution(1920, 1080);

    // Init raylib
    InitWindow(context.getWidth(), context.getHeight(), "Tower Defense");
    HideCursor();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    InitAudioDevice();

    Texture2D cursorTexture = LoadTexture("../assets/ui/cursor.png");
    if (cursorTexture.id == 0)
        std::cout << "Erreur chargement texture cursor.png\n";

    // Manager de states
    StateManager stateManager(context);

    // Premier état
    if (START_IN_SANDBOX)
        stateManager.pushState<LevelSelectionState>();
    else
        stateManager.pushState<MenuState>();

    // Boucle principale
    while (!WindowShouldClose())
    {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        // Update logique
        stateManager.update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        // Render
        stateManager.render();
        DrawTexture(cursorTexture, mouse.x - cursorTexture.width / 2, mouse.y - cursorTexture.height / 2, WHITE);

        EndDrawing();

        // Update musique (important pour raylib)
        context.updateMusic();
    }

    // Nettoyage propre
    context.unloadAllTextures();
    CloseAudioDevice();
    UnloadTexture(cursorTexture);
    CloseWindow();

    return 0;
}