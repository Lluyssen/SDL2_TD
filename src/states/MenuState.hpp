#pragma once

#include "../utils/StateManager.hpp"
#include "raylib.h"
#include "GameState.hpp"
#include "../ui/Starfield.hpp"
#include "menu/MenuBackground.hpp"
#include "menu/MenuButtons.hpp"
#include "menu/MenuNPC.hpp"
#include "menu/MenuTitle.hpp"
#include "../utils/MusicManager.hpp"

// État du menu principal : gère fond animé, étoiles, UI, PNJ et musique.
class MenuState : public IGameState
{
private:
    // Phases internes du menu
    enum class Phase
    {
        Loading,  // attente avant l’apparition de l’UI
        UIReveal, // animation d'entrée des boutons
        Idle      // menu interactif
    };

    Phase _phase = Phase::Loading;
    float _timer = 0;

    // Sous-systèmes du menu
    MenuBackground _background;
    MenuButtons _buttons;
    MenuTitle _title;
    MenuNPC _npc;

    // ECS utilisé pour le champ d'étoiles (3 couches de profondeur)
    Registry<StarComponents> _starRegistry;
    std::unique_ptr<StarfieldSystem> _farStars;
    std::unique_ptr<StarfieldSystem> _midStars;
    std::unique_ptr<StarfieldSystem> _nearStars;

    // Loading screen
    Texture2D _loadingTexture{};
    bool _loading = true;

    // Chargement progressif des frames du background
    int _bgFramesLoaded = 0;
    const int _bgTotalFrames = 41;

public:
    MenuState() = default;

    // Initialisation de l’état
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        // Image de l'écran de chargement
        _loadingTexture = LoadTexture("../assets/ui/loading.png");

        _loading = true;
        _bgFramesLoaded = 0;

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Initialisation UI et musique
        _buttons.init(ctx);
        _npc.init();
        ctx.initMusic("../assets/audio/menu_music.mp3");

        // Initialisation ECS étoiles
        _starRegistry = Registry<StarComponents>();

        _farStars = std::make_unique<StarfieldSystem>(w, h);
        _midStars = std::make_unique<StarfieldSystem>(w, h);
        _nearStars = std::make_unique<StarfieldSystem>(w, h);

        spawnStarfield(_starRegistry, 80, w, h, 1.f, 2.f);
        spawnStarfield(_starRegistry, 40, w, h, 2.f, 3.f);
        spawnStarfield(_starRegistry, 20, w, h, 3.f, 5.f);

        _phase = Phase::Loading;
        _timer = 0;
    }

    // Mise à jour logique du menu
    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();

        // Chargement progressif du background pour éviter un freeze
        if (_loading)
        {
            if (_bgFramesLoaded < _bgTotalFrames)
            {
                _background.loadFrame(ctx, "../assets/ui/bg/frame", _bgFramesLoaded);
                _bgFramesLoaded++;
            }
            else
            {
                _background.finalize();
                _loading = false;
            }

            return;
        }

        if (dt > 0.05f)
            dt = 0.05f;

        //_background.update(dt);
        ctx.updateMusic();
        _npc.update(dt);

        // Mise à jour des étoiles
        _farStars->update(dt, _starRegistry);
        _midStars->update(dt, _starRegistry);
        _nearStars->update(dt, _starRegistry);

        // Machine d’état du menu
        switch (_phase)
        {
        case Phase::Loading:

            _timer += dt;

            if (_timer > 0.2f)
            {
                _buttons.resetAnimations();
                _phase = Phase::UIReveal;
            }

            break;

        case Phase::UIReveal:

            _buttons.update(dt);

            if (_buttons.enterFinished())
                _phase = Phase::Idle;

            break;

        case Phase::Idle:
        {
            int action = _buttons.update(dt);

            if (action >= 0)
                activate(sm, action);
        }
        break;
        }
    }

    // Rendu graphique
    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Écran de chargement
        if (_loading)
        {
            ClearBackground(BLACK);

            Rectangle src{
                0,
                0,
                (float)_loadingTexture.width,
                (float)_loadingTexture.height};

            Rectangle dst{
                0,
                0,
                (float)w,
                (float)h};

            DrawTexturePro(_loadingTexture, src, dst, {0, 0}, 0, WHITE);

            float progress = (float)_bgFramesLoaded / (float)_bgTotalFrames;
            progress = (progress < 0) ? 0 : (progress > 1) ? 1 : progress;

            int barWidth = 400;
            int barHeight = 20;

            int x = w / 2 - barWidth / 2;
            int y = h / 2 + 240;

            // Barre de progression
            DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);
            DrawRectangle(x, y, barWidth * progress, barHeight, GOLD);
            DrawRectangleLines(x, y, barWidth, barHeight, WHITE);

            DrawText(
                TextFormat("Loading %d%%", (int)(progress * 100)),
                x + barWidth / 2 - 50,
                y + 30,
                20,
                WHITE);

            return;
        }

        // Rendu normal du menu
        _background.draw(w, h);

        _farStars->draw(_starRegistry);
        _midStars->draw(_starRegistry);
        _nearStars->draw(_starRegistry);

        _title.draw(w, h);

        if (_phase == Phase::UIReveal || _phase == Phase::Idle)
            _buttons.draw();

        _npc.draw(h);
    }

    // Action déclenchée par un bouton
    void activate(StateManager &sm, int id)
    {
        switch (id)
        {
        case 0:
            sm.changeState<GameState>();
            break;

        case 2:
            CloseWindow();
            break;
        }
    }

    // Nettoyage lors de la sortie de l’état
    void onExit(StateManager &) override
    {
        UnloadTexture(_loadingTexture);
    }
};