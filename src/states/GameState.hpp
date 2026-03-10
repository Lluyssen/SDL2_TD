#pragma once

#include <core/StateManager.hpp>
#include "raylib.h"
#include <states/game/LevelNode.hpp>
#include <ui/petitMenu/PetitMenu.hpp>
#include <states/PauseMenuState.hpp>
#include <states/MapState.hpp>

class GameState : public IGameState
{
private:
    Texture2D _mapTexture{};
    AnimatedSprite _lockAnim;

    std::vector<LevelNode> _levels;
    Vector2 _bgOffset{0.f, 0.f};

    PetitMenu _tooltip;
    Texture2D _tooltipTexture{};

    static constexpr float ZOOM = 1.05f;
    static constexpr float PARALLAX_AMOUNT = 40.f;
    static constexpr float PARALLAX_SPEED = 4.f;

    // conversion map -> écran
    Vector2 worldToScreen(Vector2 world, int w, int h, float drawW, float drawH) const
    {
        float scaleX = drawW / (float)_mapTexture.width;
        float scaleY = drawH / (float)_mapTexture.height;

        return {
            world.x * scaleX + (w - drawW) * 0.5f + _bgOffset.x,
            world.y * scaleY + (h - drawH) * 0.5f + _bgOffset.y};
    }

public:
    void onEnter(StateManager &sm) override
    {
        auto &ctx = sm.getContext();

        _mapTexture = LoadTexture("../assets/ui/map.png");
        _lockAnim.loadSheet(ctx, "../assets/ui/flag/Graysprite.png", 6, 0.2f);

        _tooltipTexture = LoadTexture("../assets/ui/papyrus.png");
        _tooltip.init(_tooltipTexture);

        // Définition des niveaux
        _levels = {
            {0, {160, 120}},
            {1, {700, 650}},
            {2, {1100, 250}},
            {3, {1020, 740}},
            {4, {300, 700}}};

        for (auto &n : _levels)
            n.init(ctx);
    }

    void update(StateManager &sm, float dt) override
    {
        auto &ctx = sm.getContext();
        dt = std::min(dt, 0.05f); // clamp dt pour stabilité

        Vector2 mouse = GetMousePosition();
        int w = ctx.getWidth();
        int h = ctx.getHeight();
        float drawW = w * ZOOM;
        float drawH = h * ZOOM;
        bool tooltipVisible = false;

        if (IsKeyPressed(KEY_ESCAPE))
        {
            sm.pushState<PauseMenuState>();
            return;
        }

        int unlocked = ctx.getHighestUnlockedLevel();

        // Mise à jour des nodes
        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;
            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);

            if (node.update(dt, mouse, pos, unlockedNode))
                sm.changeState<MapState>();

            if (!tooltipVisible && CheckCollisionPointCircle(mouse, pos, 30))
            {
                std::vector<std::string> lines = {
                    TextFormat("Level %d", node.id() + 1),
                    "Difficulty: Easy",
                    "Reward: 200 gold"};
                _tooltip.show({pos.x, pos.y - 40}, lines);
                tooltipVisible = true;
            }
        }

        if (!tooltipVisible)
            _tooltip.hide();

        // Parallaxe souris
        float nx = (mouse.x / w) - 0.5f;
        float ny = (mouse.y / h) - 0.5f;
        _bgOffset.x += (-nx * PARALLAX_AMOUNT - _bgOffset.x) * PARALLAX_SPEED * dt;
        _bgOffset.y += (-ny * PARALLAX_AMOUNT - _bgOffset.y) * PARALLAX_SPEED * dt;

        _lockAnim.update(dt);
        ctx.updateMusic();
    }

    void render(StateManager &sm) override
    {
        auto &ctx = sm.getContext();
        int w = ctx.getWidth();
        int h = ctx.getHeight();

        float drawW = w * ZOOM;
        float drawH = h * ZOOM;

        ClearBackground(BLACK);

        // Dessin de la map
        Rectangle src{0.f, 0.f, (float)_mapTexture.width, (float)_mapTexture.height};
        Rectangle dst{(w - drawW) * 0.5f + _bgOffset.x, (h - drawH) * 0.5f + _bgOffset.y, drawW, drawH};
        DrawTexturePro(_mapTexture, src, dst, {0.f, 0.f}, 0.f, WHITE);

        int unlocked = ctx.getHighestUnlockedLevel();

        for (auto &node : _levels)
        {
            bool unlockedNode = node.id() <= unlocked;
            Vector2 pos = worldToScreen(node.position(), w, h, drawW, drawH);
            node.draw(pos, _lockAnim, unlockedNode);
        }

        _tooltip.draw(w, h);
    }

    void onExit(StateManager &) override
    {
        UnloadTexture(_mapTexture);
        UnloadTexture(_tooltipTexture);
    }
};