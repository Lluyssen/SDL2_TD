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
        Loading,
        UIReveal,
        Idle
    };

    Phase _phase = Phase::Loading;
    float _timer = 0.f;

    // Sous-systèmes du menu
    MenuBackground _background;
    MenuButtons _buttons;
    MenuTitle _title;
    MenuNPC _npc;

    // ECS étoiles
    Registry<StarComponents> _starRegistry;
    std::unique_ptr<StarfieldSystem> _farStars;
    std::unique_ptr<StarfieldSystem> _midStars;
    std::unique_ptr<StarfieldSystem> _nearStars;

    // Loading screen
    Texture2D _loadingTexture{};
    int _bgFramesLoaded = 0;
    static constexpr int BG_TOTAL_FRAMES = 41;

    // Constantes pour la barre de progression
    static constexpr float BAR_WIDTH_PERCENT = 0.4f;
    static constexpr float BAR_HEIGHT_PERCENT = 0.03f;
    static constexpr float BAR_Y_POS_PERCENT = 0.85f;

    // Enum pour les boutons
    enum class ButtonID
    {
        Play = 0,
        Options = 1,
        Quit = 2
    };

public:
    MenuState() = default;

    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _loadingTexture = LoadTexture("../assets/ui/loading.png");
        _bgFramesLoaded = 0;

        int w = ctx.getWidth();
        int h = ctx.getHeight();

        _buttons.init(ctx);
        _npc.init();
        ctx.initMusic("../assets/audio/menu_music.mp3");

        _starRegistry = Registry<StarComponents>();
        _farStars = std::make_unique<StarfieldSystem>(w, h);
        _midStars = std::make_unique<StarfieldSystem>(w, h);
        _nearStars = std::make_unique<StarfieldSystem>(w, h);

        spawnStarfield(_starRegistry, 80, w, h, 1.f, 2.f);
        spawnStarfield(_starRegistry, 40, w, h, 2.f, 3.f);
        spawnStarfield(_starRegistry, 20, w, h, 3.f, 5.f);

        _phase = Phase::Loading;
        _timer = 0.f;
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();
        if (dt > 0.05f)
            dt = 0.05f; // Clamp global pour stabilité

        // Chargement progressif du background (2 frames par update pour fluidité)
        if (_bgFramesLoaded < BG_TOTAL_FRAMES)
        {
            for (int i = 0; i < 2 && _bgFramesLoaded < BG_TOTAL_FRAMES; ++i)
                _background.loadFrame(ctx, "../assets/ui/bg/frame", _bgFramesLoaded++);
            if (_bgFramesLoaded == BG_TOTAL_FRAMES)
                _background.finalize();

            // Mettre à jour les boutons pendant loading pour PixelReveal/ScaleHover
            _buttons.update(dt);

            // Fond animé pendant loading
            _background.update(dt);

            return;
        }

        // Fond et musique
        _background.update(dt);
        ctx.updateMusic();
        _npc.update(dt);

        // Étoiles
        _farStars->update(dt, _starRegistry);
        _midStars->update(dt, _starRegistry);
        _nearStars->update(dt, _starRegistry);

        // Machine d’état
        switch (_phase)
        {
        case Phase::Loading:
            _timer += dt;
            if (_timer > 0.2f)
            {
                _buttons.resetAnimations(); // prépare PixelReveal
                _phase = Phase::UIReveal;
            }
            break;

        case Phase::UIReveal:
            _buttons.update(dt); // continue PixelReveal
            if (_buttons.enterFinished())
                _phase = Phase::Idle;
            break;

        case Phase::Idle:
        {
            int action = _buttons.update(dt); // ScaleHover + interaction
            if (action >= 0)
                activate(sm, static_cast<ButtonID>(action));
        }
        break;
        }
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        // Écran de chargement
        if (_bgFramesLoaded < BG_TOTAL_FRAMES)
        {
            ClearBackground(BLACK);

            Rectangle src{0.f, 0.f, (float)_loadingTexture.width, (float)_loadingTexture.height};
            Rectangle dst{0.f, 0.f, (float)w, (float)h};
            DrawTexturePro(_loadingTexture, src, dst, {0, 0}, 0.f, WHITE);

            float progress = std::clamp((float)_bgFramesLoaded / BG_TOTAL_FRAMES, 0.f, 1.f);

            int barWidth = (int)(w * BAR_WIDTH_PERCENT);
            int barHeight = (int)(h * BAR_HEIGHT_PERCENT);
            int x = w / 2 - barWidth / 2;
            int y = (int)(h * BAR_Y_POS_PERCENT);

            DrawRectangle(x, y, barWidth, barHeight, DARKGRAY);
            DrawRectangle(x, y, (int)(barWidth * progress), barHeight, GOLD);
            DrawRectangleLines(x, y, barWidth, barHeight, WHITE);

            const int percent = (int)(progress * 100);
            const std::string text = TextFormat("Loading %d%%", percent);
            int textWidth = MeasureText(text.c_str(), 20);
            DrawText(text.c_str(), x + barWidth / 2 - textWidth / 2, y + barHeight + 10, 20, WHITE);

            _background.draw(w, h); // fond partiellement chargé

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

    void activate(StateManager &sm, ButtonID id)
    {
        switch (id)
        {
        case ButtonID::Play:
            sm.changeState<GameState>();
            break;
        case ButtonID::Quit:
            CloseWindow();
            break;
        default:
            break;
        }
    }

    void onExit(StateManager &) override
    {
        UnloadTexture(_loadingTexture);
    }
};